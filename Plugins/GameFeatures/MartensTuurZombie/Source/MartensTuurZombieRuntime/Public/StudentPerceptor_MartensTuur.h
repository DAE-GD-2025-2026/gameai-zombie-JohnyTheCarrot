// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FKnownHouse_MartensTuur.h"
#include "SurvivorAgentBehavior_MartensTuur.h"
#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"
#include "Components/ActorComponent.h"
#include "Items/Food.h"
#include "Items/ItemType.h"
#include "Items/Medkit.h"
#include "Items/Weapon.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"
#include "Zombies/BaseZombie.h"
#include "StudentPerceptor_MartensTuur.generated.h"

UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MARTENSTUURZOMBIERUNTIME_API UStudentPerceptor_MartensTuur : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStudentPerceptor_MartensTuur();
	
	UPROPERTY()
	USurvivorAgentBehavior_MartensTuur *BehaviorComp;
	
	UPROPERTY()
	TMap<TWeakObjectPtr<ABaseZombie>, FVector> LastSeenZombiePos{};
	
	UPROPERTY(BlueprintReadOnly)
	TArray<ABaseItem*> DesiredItems{};
	
	UPROPERTY(BlueprintReadOnly)
	TArray<AWeapon*> KnownWeapons{};
	
	UPROPERTY(BlueprintReadOnly)
	TArray<AFood*> KnownFoods{};
	
	UPROPERTY(BlueprintReadOnly)
	TArray<AMedkit*> KnownMedkits{};
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
