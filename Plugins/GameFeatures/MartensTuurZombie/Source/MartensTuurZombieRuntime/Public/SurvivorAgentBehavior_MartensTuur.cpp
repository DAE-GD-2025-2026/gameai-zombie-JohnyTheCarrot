#include "SurvivorAgentBehavior_MartensTuur.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Common/InventoryComponent.h"
#include "SteeringBehaviors_MartensTuur/SteeringBehavior_MartensTuur.h"

TOptional<int> USurvivorAgentBehavior_MartensTuur::GetFreeInvSlot() const
{
	auto const &Inv = InventoryComp->GetInventory();
	for (int Idx = 0; Idx < Inv.Num(); ++Idx)
	{
		if (Inv[Idx] == nullptr) return Idx;
	}
	
	return NullOpt;
}

bool USurvivorAgentBehavior_MartensTuur::InvContainsItemType(EItemType Type) const
{
	return InventoryComp->GetInventory().ContainsByPredicate([Type](ABaseItem *Item)
	{
		if (Item == nullptr) return false;
		return Item->GetItemType() == Type;
	});
}

bool USurvivorAgentBehavior_MartensTuur::AwareOfItemType(EItemType Type) const
{
	return KnownItems.ContainsByPredicate([Type](TWeakObjectPtr<ABaseItem> const &Item)
	{
		if (Item == nullptr) return false;
		return Item->GetItemType() == Type;
	});
}

AHouse* USurvivorAgentBehavior_MartensTuur::GetClosestHouse() const
{
	AHouse* Closest{};
	auto const ActorPos = GetOwner()->GetActorLocation();

	for (auto House : UncheckedHouses)
	{
		// This doesn't take into account cases where the direct distance is lower, but the path to get there is longer...
		// but honestly, it's an organic character, it doesn't need to be perfect...
		if (Closest == nullptr || FVector::DistSquared(ActorPos, House->GetBounds().Origin) < FVector::DistSquared(ActorPos, Closest->GetBounds().Origin))
		{
			Closest = House.Get();
		}
	}

	return Closest;
}

ABaseItem* USurvivorAgentBehavior_MartensTuur::GetClosestItemOfType(EItemType Type) const
{
	TWeakObjectPtr<ABaseItem> Closest{};
	auto const ActorPos = GetOwner()->GetActorLocation();
	
	for (auto const Item : KnownItems)
	{
		// TODO: this doesn't take into account cases where the direct distance is lower, but the path to get there is longer
		if (
			(Closest == nullptr
			|| FVector::DistSquared(ActorPos, Item->GetActorLocation()) < FVector::DistSquared(ActorPos, Closest->GetActorLocation())
			)
			&& Item->GetItemType() == Type
		)
		{
			Closest = Item;
		}
	}
	
	return Closest.Get();
}

void USurvivorAgentBehavior_MartensTuur::UseItem(int Idx) const
{
	auto const *Item = InventoryComp->GetInventory()[Idx];
	if (Item == nullptr) return;
	
	InventoryComp->UseItem(Idx);
	if (Item->GetValue() == 0)
	{
		InventoryComp->RemoveItem(Idx);
	}
}

bool USurvivorAgentBehavior_MartensTuur::IsHungry() const
{
	return StaminaComp->GetCurrentStamina() / StaminaComp->GetMaxStamina() <= 0.5;
}

bool USurvivorAgentBehavior_MartensTuur::IsHurt() const
{
	return HealthComp->GetHealth() < HealthComp->GetMaxHealth();
}

float USurvivorAgentBehavior_MartensTuur::ScoreItemInterest(ABaseItem* Item, float *ProximityScore) const
{
	if (Item == nullptr) return TNumericLimits<float>::Lowest();
	float Score = 0.f;
	
	switch (Item->GetItemType()) {
	case EItemType::Shotgun:
	case EItemType::Pistol:
		{
			auto const Gun = Cast<AWeapon>(Item);
			Score += Item->GetValue(); // ammo
			Score += Gun->GetDamage() * 1.5f;
			
			bool const HasGuns = InvContainsItemType(EItemType::Pistol) || InvContainsItemType(EItemType::Shotgun);
			if (!HasGuns)
				Score *= 2.f;
		}
		break;
	case EItemType::Food:
		{
			bool const HasFood = InvContainsItemType(EItemType::Food);
			Score += Item->GetValue();
			if (!HasFood)
				Score *= StaminaComp->GetMaxStamina() / StaminaComp->GetCurrentStamina();
		}
		break;
	case EItemType::Medkit:
		{
			Score = 1.5f;
			bool const HasMedkit = InvContainsItemType(EItemType::Medkit);
			Score += Item->GetValue();
			if (HasMedkit || IsHurt())
				Score *= 2.f;
		}
		break;
	case EItemType::Garbage:
		Score = 0.01f;
		break;
	}
	
	if (ProximityScore != nullptr && !Item->IsHidden())
	{
		*ProximityScore = 2.f / GetOwner()->GetSquaredHorizontalDistanceTo(Item);
	}
	
	return Score;
}

USurvivorAgentBehavior_MartensTuur::USurvivorAgentBehavior_MartensTuur()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

bool USurvivorAgentBehavior_MartensTuur::GrabItem(ABaseItem* Item)
{
	if (auto const FreeSlot = GetFreeInvSlot(); FreeSlot.IsSet())
	{
		auto const GrabWasSuccess = InventoryComp->GrabItem(FreeSlot.GetValue(), Item);
		if (GrabWasSuccess) KnownItems.Remove(Item);
		
		return GrabWasSuccess;
	}
	
	return false;
}

bool USurvivorAgentBehavior_MartensTuur::GrabDesiredItem()
{
	if (!DesiredItem.IsValid()) return false;
	
	InventoryComp->RemoveItem(DesiredItemSlot);
	if (InventoryComp->GrabItem(DesiredItemSlot, DesiredItem.Get()))
	{
		KnownItems.Remove(DesiredItem);
		DesiredItem.Reset();
		return true;
	}
	
	return false;
}

void USurvivorAgentBehavior_MartensTuur::BeginPlay()
{
	Super::BeginPlay();
	FloatingPawnMovement = GetOwner()->GetComponentByClass<UFloatingPawnMovement>();
	check(FloatingPawnMovement);
	
	HealthComp = GetOwner()->GetComponentByClass<UHealthComponent>();
	check(HealthComp.Get());
	
	StaminaComp = GetOwner()->GetComponentByClass<UStaminaComponent>();
	check(StaminaComp.Get());
	
	InventoryComp = GetOwner()->GetComponentByClass<UInventoryComponent>();
	check(InventoryComp.Get());
	
	LastHealthAmount = HealthComp->GetHealth();
}

void USurvivorAgentBehavior_MartensTuur::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                       FActorComponentTickFunction* ThisTickFunction)
{
	if (KnownZombies.IsEmpty() && HealthComp->GetHealth() < LastHealthAmount)
	{
		// oh noes
		bWasSurpriseAttacked = true;
	}
	
	KnownZombies.RemoveAll([](TWeakObjectPtr<ABaseZombie> const& Zombie)
	{
		return !Zombie.IsValid();
	});
	
	if (IsHungry())
	{
		auto const &InventoryItems = InventoryComp->GetInventory();
		for (int Idx = 0; Idx < InventoryItems.Num(); ++Idx)
		{
			auto const &InventoryItem = InventoryItems[Idx];
			if (InventoryItem == nullptr) continue;
			if (InventoryItem->GetItemType() == EItemType::Food)
			{
				UseItem(Idx);
			}
		}
	}
	
	if (IsHurt())
	{
		auto const &InventoryItems = InventoryComp->GetInventory();
		for (int Idx = 0; Idx < InventoryItems.Num(); ++Idx)
		{
			auto const &InventoryItem = InventoryItems[Idx];
			if (InventoryItem == nullptr) continue;
			if (InventoryItem->GetItemType() == EItemType::Medkit)
			{
				UseItem(Idx);
			}
		}
	}
	
	if (!DesiredItem.IsValid())
	{
		auto const &Inv = InventoryComp->GetInventory();
		float BestProximityScore{};
		for (auto const &KnownItem : KnownItems)
		{
			for (auto InvIdx = 0; InvIdx < Inv.Num(); ++InvIdx)
			{
				auto const InvItem = Inv[InvIdx];
				
				auto const InvItemScore = InvItem == nullptr ? -10.f : ScoreItemInterest(InvItem, nullptr);
				float ProximityScore{};
				if (ScoreItemInterest(KnownItem.Get(), &ProximityScore) > InvItemScore)
				{
					if (ProximityScore >= BestProximityScore)
					{
						DesiredItem = KnownItem;
						BestProximityScore = ProximityScore;
						DesiredItemSlot = InvIdx;
					}
				}
			}
		}
	} else if (DesiredItem->IsHidden())
		DesiredItem.Reset();
}

void USurvivorAgentBehavior_MartensTuur::TickSteeringBehavior(USteeringBehavior_MartensTuur *Behavior,
                                                              FSteeringBehaviorTarget_MartensTuur const &Target,
                                                              float DeltaTime, float Scale)
{
	check(Behavior != nullptr);
	auto Pawn = Cast<APawn>(GetOwner());
	if (TargetRotator.IsSet())
	{
		auto const CurrentRot = Pawn->GetActorRotation();
		
		float const RotationSpeed = 8.f;
		FRotator const NewRot = FMath::RInterpTo(
			CurrentRot,
			TargetRotator.GetValue(),
			DeltaTime,
			RotationSpeed
		);
			
		auto const Angle = FMath::FindDeltaAngleDegrees(
			CurrentRot.Yaw,
			TargetRotator.GetValue().Yaw
		);	
		if (Angle <= 2.f)
		{
			TargetRotator.Reset();
		}
		UE_LOG(LogTemp, Warning, TEXT("Angle: %f"), Angle);
		Pawn->SetActorRotation(NewRot);
	}
	
	auto const Output = Behavior->CalculateOutput(DeltaTime, Target, Cast<ASurvivorPawn>(GetOwner()));
	auto const bIsDone = Behavior->CheckIfDone(Output, DeltaTime, Target, GetOwner());
	if (bIsDone)
	{
		Behavior->Finish();
	}
	
	if (Output.Direction.SquaredLength() > KINDA_SMALL_NUMBER)
	{
		auto const MoveDir = Get3DVec(Output.Direction.GetSafeNormal());
		FVector const Movement{MoveDir * FloatingPawnMovement->GetMaxSpeed() * Scale};
		UE_LOG(LogTemp, Warning, TEXT("Moving %f, %f * %f"), Movement.X, Movement.Y, Scale);
		Pawn->AddMovementInput(Movement);
	}
	
	if (Output.FacingTowards.IsSet())
	{
		TargetRotator = Output.FacingTowards.GetValue();
	}
}

void USurvivorAgentBehavior_MartensTuur::Shoot()
{
	auto const &Inv = InventoryComp->GetInventory();
	int GunIdx{-1};
	for (int Idx = 0; Idx < Inv.Num(); ++Idx)
	{
		if (Inv[Idx] != nullptr && (Inv[Idx]->GetItemType() == EItemType::Pistol || Inv[Idx]->GetItemType() == EItemType::Shotgun))
		{
			GunIdx = Idx;
		}
	}
	
	if (GunIdx == -1) return;
	
	UseItem(GunIdx);
}

float USurvivorAgentBehavior_MartensTuur::GetSafeEnemyDistance() const
{
	if (InvContainsItemType(EItemType::Pistol))
	{
		return 500.f;
	}
	
	if (InvContainsItemType(EItemType::Shotgun))
	{
		return 200.f;
	}
	
	return 3000.f;
}

void USurvivorAgentBehavior_MartensTuur::InformAboutHouse(AHouse* House)
{
	if (UncheckedHouses.Contains(House)) return;
	if (CheckedHouses.Contains(House)) return;
	
	GEngine->AddOnScreenDebugMessage(7, 5.f, FColor::Yellow, 
	FString::Printf(TEXT("Saw House!!!!!")));
	UncheckedHouses.Add(House);
}

void USurvivorAgentBehavior_MartensTuur::InformAboutItem(ABaseItem* Item)
{
	if (KnownItems.Contains(Item)) return;
	KnownItems.Add(Item);
}

void USurvivorAgentBehavior_MartensTuur::MarkChecked(AHouse* House)
{
	UncheckedHouses.Remove(House);
	CheckedHouses.Add(House);
}

void USurvivorAgentBehavior_MartensTuur::HasSeenZombie(ABaseZombie* Zombie)
{
	if (!KnownZombies.Contains(Zombie))
	{
		KnownZombies.Add(Zombie);
	}
}

void USurvivorAgentBehavior_MartensTuur::UpdateBlackboard(UBlackboardComponent* Blackboard) const
{
	if (Blackboard == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Blackboard is nullptr"));
		return;
	}
	
	auto const FreeSlot = GetFreeInvSlot();
	Blackboard->SetValueAsBool(FName{"HasInventorySpace"}, FreeSlot.IsSet());
	if (auto const ClosestHouse{GetClosestHouse()}; ClosestHouse != nullptr)
		Blackboard->SetValueAsObject(FName{"NewHouse"}, ClosestHouse);
	else
		Blackboard->ClearValue(FName{"NewHouse"});
	
	Blackboard->SetValueAsBool(FName{"SeesZombies"}, !KnownZombies.IsEmpty());
	Blackboard->SetValueAsBool(FName{"WasSurpriseAttacked"}, bWasSurpriseAttacked);
	bWasSurpriseAttacked = false;
	
	Blackboard->SetValueAsVector(FName{"SelfPos"}, GetOwner()->GetActorLocation());
	
	FName const WantedItem{"WantedItem"};
	auto const HasGun = InvContainsItemType(EItemType::Pistol) || InvContainsItemType(EItemType::Shotgun);
	Blackboard->SetValueAsBool(FName{"HasGun"}, HasGun);
	auto const HasFood = !InvContainsItemType(EItemType::Food);
	Blackboard->SetValueAsBool(FName{"HasFood"}, HasFood);
	
	if (DesiredItem.Get() != nullptr || KnownItems.Contains(DesiredItem))
		Blackboard->SetValueAsObject(WantedItem, DesiredItem.Get());
	else
		Blackboard->ClearValue(WantedItem);
}
