// Fill out your copyright notice in the Description page of Project Settings.


#include "StudentPerceptor.h"

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
	GoapComp->State.AwareOf.HousesNum = KnownHouses.Num();
	GoapComp->State.AwareOf.FoodNum = KnownFoods.Num();
	GoapComp->State.AwareOf.MedkitsNum = KnownMedkits.Num();
	GoapComp->State.AwareOf.UncheckedHousesNum = 0;
	for (auto const &House : KnownHouses)
	{
		if (!House.HasChecked)
			++GoapComp->State.AwareOf.UncheckedHousesNum;
	}
	
	// TODO: enemies
	
	GoapComp->State.UpdateFlags();
	
	// for (auto const &Item : KnownItems)
	// {
	// 	if (Item.Type == EItemType::Pistol || Item.Type == EItemType::Shotgun)
	// 		EnumAddFlags(SurvivorState, EGOAPFlags_Martens_Tuur::HasFoundWeapon);
	// }
	// if (!KnownHouses.IsEmpty())
	// 	EnumAddFlags(SurvivorState, EGOAPFlags_Martens_Tuur::HasFoundHouse);
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

FKnownHouse_MartensTuur *UStudentPerceptor::GetClosestHouse(bool bAllowChecked)
{
	FKnownHouse_MartensTuur* Closest{};
	auto const ActorPos = GetOwner()->GetActorLocation();

	for (auto &House : KnownHouses)
	{
		if (House.HasChecked && !bAllowChecked) continue;
		
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
}

void UStudentPerceptor::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Green, 
	FString::Printf(TEXT("Saw Something!")));

	if (AHouse *House = Cast<AHouse>(Actor))
	{
		FKnownHouse_MartensTuur const KnownHouse{.Bounds = House->GetBounds()};
		if (KnownHouses.Find(KnownHouse) != INDEX_NONE) return;
		
		GEngine->AddOnScreenDebugMessage(6, 1.f, FColor::Yellow, 
		FString::Printf(TEXT("Saw House!!!!!")));
		KnownHouses.Add(KnownHouse);
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
