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
	
	BehaviorComp = GetOwner()->GetComponentByClass<USurvivorAgentBehavior_MartensTuur>();
	
}

void UStudentPerceptor_MartensTuur::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Green, 
	FString::Printf(TEXT("Saw Something!")));
	if (BehaviorComp == nullptr)
	{
		BehaviorComp = GetOwner()->GetComponentByClass<USurvivorAgentBehavior_MartensTuur>();
		check(BehaviorComp);
	}

	if (AHouse *House = Cast<AHouse>(Actor))
	{
		BehaviorComp->InformAboutHouse(House);
	}
	else if (ABaseZombie *Zombie = Cast<ABaseZombie>(Actor))
	{
		BehaviorComp->HasSeenZombie(Zombie);
	}
	else if (ABaseItem *Item = Cast<ABaseItem>(Actor))
	{
		BehaviorComp->InformAboutItem(Item);
	}
}

void UStudentPerceptor_MartensTuur::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
