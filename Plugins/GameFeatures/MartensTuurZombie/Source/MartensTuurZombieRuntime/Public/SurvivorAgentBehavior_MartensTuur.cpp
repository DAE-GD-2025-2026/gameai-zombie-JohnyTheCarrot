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

bool USurvivorAgentBehavior_MartensTuur::ContainsItemType(EItemType Type) const
{
	return InventoryComp->GetInventory().ContainsByPredicate([Type](ABaseItem *Item)
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
}

void USurvivorAgentBehavior_MartensTuur::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                                       FActorComponentTickFunction* ThisTickFunction)
{
	if (CurrentSteeringBehavior == nullptr) return;
	
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
	
	auto const Output = CurrentSteeringBehavior->CalculateOutput(DeltaTime, SteerTarget, Cast<ASurvivorPawn>(GetOwner()));
	auto const bIsDone = CurrentSteeringBehavior->CheckIfDone(Output, DeltaTime, SteerTarget, GetOwner());
	if (bIsDone)
	{
		CurrentSteeringBehavior->Finish();
		return;
	}
	
	if (Output.Direction.SquaredLength() > KINDA_SMALL_NUMBER)
	{
		auto const MoveDir = Get3DVec(Output.Direction.GetSafeNormal());
		FVector const Movement{MoveDir * FloatingPawnMovement->GetMaxSpeed()};
		Pawn->AddMovementInput(Movement, Output.SpeedScale);
	}
	
	if (Output.FacingTowards.IsSet())
	{
		TargetRotator = Output.FacingTowards.GetValue();
	}
}

USteeringBehavior_MartensTuur* USurvivorAgentBehavior_MartensTuur::GetSteeringBehavior() const
{
	return CurrentSteeringBehavior;
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
	
	FName const WantedItem{"WantedItem"};
	auto const HasGun = ContainsItemType(EItemType::Pistol) || ContainsItemType(EItemType::Shotgun);
	if (!HasGun)
	{
		auto const ClosestPistol = GetClosestItemOfType(EItemType::Pistol);
		auto const PistolDistance = ClosestPistol != nullptr ? ClosestPistol->GetDistanceTo(GetOwner()) : MAX_int32;
		auto const ClosestShotgun = GetClosestItemOfType(EItemType::Shotgun);
		auto const ShotgunDistance = ClosestShotgun != nullptr ? ClosestShotgun->GetDistanceTo(GetOwner()) : MAX_int32;
		
		auto const ClosestOfTwo = PistolDistance < ShotgunDistance
			? ClosestPistol
			: ClosestShotgun;
		if (ClosestOfTwo != nullptr)
			Blackboard->SetValueAsObject(WantedItem, ClosestOfTwo);
	}
	
	auto const CurrentWantedItem = Blackboard->GetValueAsObject(WantedItem);
	if (!KnownItems.Contains(CurrentWantedItem)) Blackboard->ClearValue(WantedItem);
}

void USurvivorAgentBehavior_MartensTuur::SetCurrentSteeringBehavior(USteeringBehavior_MartensTuur* SteeringBehavior)
{
	check(SteeringBehavior);
	CurrentSteeringBehavior = SteeringBehavior;
	CurrentSteeringBehavior->Reset();
}
