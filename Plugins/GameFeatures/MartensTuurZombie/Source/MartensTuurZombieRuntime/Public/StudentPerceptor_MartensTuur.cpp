// Fill out your copyright notice in the Description page of Project Settings.


#include "StudentPerceptor_MartensTuur.h"

#include "Common/InventoryComponent.h"
#include "Items/BaseItem.h"
#include "Village/House/House.h"
#include "Zombies/BaseZombie.h"


UStudentPerceptor_MartensTuur::UStudentPerceptor_MartensTuur()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptor_MartensTuur::MarkHouseChecked(AHouse* House)
{
}

void UStudentPerceptor_MartensTuur::MarkChecked(AHouse* House)
{
	UncheckedHouses.Remove(House);
	CheckedHouses.Add(House);
}

// TODO: get closest has too many variants, reduce repetition!
TWeakObjectPtr<AWeapon> UStudentPerceptor_MartensTuur::GetClosestWeapon() const
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

TWeakObjectPtr<AMedkit> UStudentPerceptor_MartensTuur::GetClosestMedkit() const
{
	TWeakObjectPtr<AMedkit> Closest{};
	auto const ActorPos = GetOwner()->GetActorLocation();
	
	for (auto const &Item : KnownMedkits)
	{
		// TODO: this doesn't take into account cases where the direct distance is lower, but the path to get there is longer
		if (Closest == nullptr || FVector::DistSquared(ActorPos, Item->GetActorLocation()) < FVector::DistSquared(ActorPos, Closest->GetActorLocation()))
		{
			Closest = Item;
		}
	}
	
	return Closest;
}

AHouse *UStudentPerceptor_MartensTuur::GetClosestHouse()
{
	AHouse* Closest{};
	auto const ActorPos = GetOwner()->GetActorLocation();

	for (AHouse *const House : UncheckedHouses)
	{
		// This doesn't take into account cases where the direct distance is lower, but the path to get there is longer...
		// but honestly, it's an organic character, it doesn't need to be perfect...
		if (Closest == nullptr || FVector::DistSquared(ActorPos, House->GetBounds().Origin) < FVector::DistSquared(ActorPos, Closest->GetBounds().Origin))
		{
			Closest = House;
		}
	}

	return Closest;
}

TWeakObjectPtr<AFood> UStudentPerceptor_MartensTuur::GetClosestFood() const
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

void UStudentPerceptor_MartensTuur::BeginPlay()
{
	Super::BeginPlay();
	
	auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>();
	check(PerceptionComp);
	PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptor_MartensTuur::OnPerceptionUpdated);
	
	HealthComp = GetOwner()->GetComponentByClass<UHealthComponent>();
	check(HealthComp);
	
	StaminaComp = GetOwner()->GetComponentByClass<UStaminaComponent>();
	check(StaminaComp);
	
	InventoryComp = GetOwner()->GetComponentByClass<UInventoryComponent>();
	check(InventoryComp);
}

void UStudentPerceptor_MartensTuur::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Green, 
	FString::Printf(TEXT("Saw Something!")));

	if (AHouse *House = Cast<AHouse>(Actor))
	{
		if (UncheckedHouses.Find(House) != INDEX_NONE) return;
		if (CheckedHouses.Find(House) != INDEX_NONE) return;
		
		GEngine->AddOnScreenDebugMessage(7, 5.f, FColor::Yellow, 
		FString::Printf(TEXT("Saw House!!!!!")));
		UncheckedHouses.Add(House);
		
		PlacesToCheckForHouse.Add(House->GetBounds().Origin - FVector{House->GetBounds().Extent.X, 0.f, 0.f});
		PlacesToCheckForHouse.Add(House->GetBounds().Origin + FVector{House->GetBounds().Extent.X, 0.f, 0.f});
		PlacesToCheckForHouse.Add(House->GetBounds().Origin - FVector{0.f, House->GetBounds().Extent.Y, 0.f});
		PlacesToCheckForHouse.Add(House->GetBounds().Origin + FVector{0.f, House->GetBounds().Extent.Y, 0.f});
	}
	else if (ABaseZombie *Zombie = Cast<ABaseZombie>(Actor))
	{
		auto &LastPos = LastSeenZombiePos.FindOrAdd(Zombie, Zombie->GetActorLocation());
		LastPos = Zombie->GetActorLocation();
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
			auto const Medkit = Cast<AMedkit>(Item);
			if (KnownMedkits.Find(Medkit) != INDEX_NONE) return;
			
			KnownMedkits.Add(Medkit);
		}
	}
}

void UStudentPerceptor_MartensTuur::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
