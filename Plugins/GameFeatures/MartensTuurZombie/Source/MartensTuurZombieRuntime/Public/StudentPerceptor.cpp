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

FVector const *UStudentPerceptor::GetClosestWeapon() const
{
	FVector const* Closest{};
	auto const ActorPos = GetOwner()->GetActorLocation();
	
	for (auto const &Item : KnownWeapons)
	{
		// TODO: this doesn't take into account cases where the direct distance is lower, but the path to get there is longer
		if (Closest == nullptr || FVector::DistSquared(ActorPos, Item) < FVector::DistSquared(ActorPos, *Closest))
		{
			Closest = &Item;
		}
	}
	
	return Closest;
}

FVector const* UStudentPerceptor::GetClosestHouse() const
{
	FVector const* Closest{};
	auto const ActorPos = GetOwner()->GetActorLocation();

	for (auto const &House : KnownHouses)
	{
		// This doesn't take into account cases where the direct distance is lower, but the path to get there is longer...
		// but honestly, it's an organic character, it doesn't need to be perfect...
		if (Closest == nullptr || FVector::DistSquared(ActorPos, House.Location) < FVector::DistSquared(ActorPos, *Closest))
		{
			Closest = &House.Location;
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
		FKnownHouse const KnownHouse{.Location = House->GetActorLocation()};
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
			FVector const KnownWeapon{Item->GetActorLocation()};
			if (KnownWeapons.Find(KnownWeapon) != INDEX_NONE) return;
			
			KnownWeapons.Add(KnownWeapon);
			return;
		}
		
		if (Type == EItemType::Food)
		{
			FVector const KnownFood{Item->GetActorLocation()};
			if (KnownFoods.Find(KnownFood) != INDEX_NONE) return;
			
			KnownFoods.Add(KnownFood);
			return;
		}
		
		if (Type == EItemType::Medkit)
		{
			auto const Loc = Item->GetActorLocation();
			if (KnownMedkits.Find(Loc) != INDEX_NONE) return;
			
			KnownMedkits.Add(Loc);
			return;
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
