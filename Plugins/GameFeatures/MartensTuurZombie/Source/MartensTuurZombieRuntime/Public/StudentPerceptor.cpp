// Fill out your copyright notice in the Description page of Project Settings.


#include "StudentPerceptor.h"

#include "Items/BaseItem.h"
#include "Village/House/House.h"


void UStudentPerceptor::RefreshSurvivorState()
{
	SurvivorState = {};
	
	for (auto const &Item : KnownItems)
	{
		if (Item.Type == EItemType::Pistol || Item.Type == EItemType::Shotgun) SurvivorState.HasFoundWeapon = true;
	}
	SurvivorState.HasFoundHouse = !KnownHouses.IsEmpty();
}

UStudentPerceptor::UStudentPerceptor()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStudentPerceptor::BeginPlay()
{
	Super::BeginPlay();
	
	if (auto PerceptionComp = GetOwner()->GetComponentByClass<UAIPerceptionComponent>())
	{
		PerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &UStudentPerceptor::OnPerceptionUpdated);
	}
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
		FKnownItem const KnownItem{.Type = Item->GetItemType(), .Location = Item->GetActorLocation()};
		// check if we already know the item
		if (KnownItems.Find(KnownItem) != INDEX_NONE) return;
		
		KnownItems.Add(KnownItem);
	}
	else return;
	
	RefreshSurvivorState();
}
