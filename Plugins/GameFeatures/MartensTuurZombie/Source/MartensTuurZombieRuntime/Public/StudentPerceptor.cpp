// Fill out your copyright notice in the Description page of Project Settings.


#include "StudentPerceptor.h"

#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "Village/House/House.h"


void UStudentPerceptor::RefreshSurvivorState()
{
	if (!GoapComp)
	{
		UE_LOG(LogTemp, Warning, TEXT("No GoapComp"));
		GoapComp = GetOwner()->GetComponentByClass<UGoapGraph>();
		return;
	}
	
	GoapComp->State.Health = HealthComp->GetHealth() / HealthComp->GetMaxHealth();
	GoapComp->State.Stamina = StaminaComp->GetCurrentStamina() / StaminaComp->GetMaxStamina();
	GoapComp->State.AwareOf.WeaponsNum = KnownWeapons.Num();
	GoapComp->State.AwareOf.HousesNum = CheckedHouses.Num();
	GoapComp->State.AwareOf.FoodNum = KnownFoods.Num();
	GoapComp->State.AwareOf.MedkitsNum = KnownMedkits.Num();
	GoapComp->State.AwareOf.UncheckedHousesNum = UncheckedHouses.Num();
	
	GoapComp->State.InventoryContains.Food = InventoryComp->GetInventory().ContainsByPredicate([](ABaseItem const *Item)
	{
		if (!Item) return false;
		return Item->GetItemType() == EItemType::Food;
	});
	GoapComp->State.InventoryContains.Weapon = InventoryComp->GetInventory().ContainsByPredicate([](ABaseItem const *Item)
	{
		if (!Item) return false;
		return Item->GetItemType() == EItemType::Pistol || Item->GetItemType() == EItemType::Shotgun;
	});
	
	// TODO: enemies
	
	auto bDidFlagsChange = GoapComp->State.UpdateFlags();
	if (bDidFlagsChange)
	{
		// auto const NewPlan = GoapComp->Plan(GoapComp->State, GoapComp->CurrentGoal.Get());
		// GoapComp->ActivatePlan(NewPlan);
	}
}

UStudentPerceptor::UStudentPerceptor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

TWeakObjectPtr<AWeapon> UStudentPerceptor::GetClosestWeapon() const
{
	TWeakObjectPtr<AWeapon> Closest{};
	auto const ActorPos = GetOwner()->GetActorLocation();
	
	for (auto const &Item : KnownWeapons)
	{
		// TODO: this doesn't take into account cases where the direct distance is lower, but the path to get there is longer
		if (Closest == nullptr || FVector::DistSquared(ActorPos, Item->GetActorLocation()) < FVector::DistSquared(ActorPos, Closest->GetActorLocation()))
		{
			Closest = Item;
		}
	}
	
	return Closest;
}

FKnownHouse_MartensTuur *UStudentPerceptor::GetClosestHouse()
{
	FKnownHouse_MartensTuur* Closest{};
	auto const ActorPos = GetOwner()->GetActorLocation();

	for (auto &House : UncheckedHouses)
	{
		// This doesn't take into account cases where the direct distance is lower, but the path to get there is longer...
		// but honestly, it's an organic character, it doesn't need to be perfect...
		if (Closest == nullptr || FVector::DistSquared(ActorPos, House.Bounds.Origin) < FVector::DistSquared(ActorPos, Closest->Bounds.Origin))
		{
			Closest = &House;
		}
	}

	return Closest;
}

TWeakObjectPtr<AFood> UStudentPerceptor::GetClosestFood() const
{
	TWeakObjectPtr<AFood> Closest{};
	auto const ActorPos = GetOwner()->GetActorLocation();
	
	for (auto const &Item : KnownFoods)
	{
		// TODO: this doesn't take into account cases where the direct distance is lower, but the path to get there is longer
		if (Closest == nullptr || FVector::DistSquared(ActorPos, Item->GetActorLocation()) < FVector::DistSquared(ActorPos, Closest->GetActorLocation()))
		{
			Closest = Item;
		}
	}
	
	return Closest;
}

void UStudentPerceptor::BeginPlay()
{
	Super::BeginPlay();
	
	auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>();
	check(PerceptionComp);
	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptor::OnPerceptionUpdated);
	
	HealthComp = GetOwner()->GetComponentByClass<UHealthComponent>();
	check(HealthComp);
	
	StaminaComp = GetOwner()->GetComponentByClass<UStaminaComponent>();
	check(StaminaComp);
	
	InventoryComp = GetOwner()->GetComponentByClass<UInventoryComponent>();
	check(InventoryComp);
}

void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Green, 
	FString::Printf(TEXT("Saw Something!")));

	if (AHouse *House = Cast<AHouse>(Actor))
	{
		FKnownHouse_MartensTuur const KnownHouse{.Bounds = House->GetBounds()};
		if (UncheckedHouses.Find(KnownHouse) != INDEX_NONE) return;
		if (CheckedHouses.Find(KnownHouse) != INDEX_NONE) return;
		
		GEngine->AddOnScreenDebugMessage(7, 5.f, FColor::Yellow, 
		FString::Printf(TEXT("Saw House!!!!!")));
		UncheckedHouses.Add(KnownHouse);
	}
	else if (ABaseItem *Item = Cast<ABaseItem>(Actor))
	{
		auto const Type = Item->GetItemType();
		if (Type == EItemType::Pistol || Type == EItemType::Shotgun)
		{
			auto const Weapon = Cast<AWeapon>(Item);
			if (KnownWeapons.Find(Weapon) != INDEX_NONE) return;
			
			KnownWeapons.Add(Weapon);
		}
		else if (Type == EItemType::Food)
		{
			if (KnownFoods.Find(Cast<AFood>(Item)) != INDEX_NONE) return;
			
			KnownFoods.Add(Cast<AFood>(Item));
			return;
		}
		else if (Type == EItemType::Medkit)
		{
			auto const Loc = Item->GetActorLocation();
			if (KnownMedkits.Find(Loc) != INDEX_NONE) return;
			
			KnownMedkits.Add(Loc);
		}
	}
	else return;
	
	RefreshSurvivorState();
}

void UStudentPerceptor::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	RefreshSurvivorState();
}
