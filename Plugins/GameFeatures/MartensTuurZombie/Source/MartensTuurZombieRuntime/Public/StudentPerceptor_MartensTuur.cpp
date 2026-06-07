// Fill out your copyright notice in the Description page of Project Settings.


#include "StudentPerceptor_MartensTuur.h"

#include "Items/BaseItem.h"
#include "Village/House/House.h"
#include "Zombies/BaseZombie.h"

UStudentPerceptor_MartensTuur::UStudentPerceptor_MartensTuur()
{
	PrimaryComponentTick.bCanEverTick = true;
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
