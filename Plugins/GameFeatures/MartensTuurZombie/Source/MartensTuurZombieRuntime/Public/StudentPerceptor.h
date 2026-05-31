// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FKnownHouse_MartensTuur.h"
#include "GOAP.h"
#include "GOAPPlanning.h"
#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"
#include "Components/ActorComponent.h"
#include "Items/Food.h"
#include "Items/ItemType.h"
#include "Items/Weapon.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"
#include "StudentPerceptor.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MARTENSTUURZOMBIERUNTIME_API UStudentPerceptor : public UActorComponent
{
	GENERATED_BODY()
	
	void RefreshSurvivorState();

public:
	// Sets default values for this component's properties
	UStudentPerceptor();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGoapGraph *GoapComp{};
	
	UPROPERTY()
	UHealthComponent *HealthComp{};
	
	UPROPERTY()
	UStaminaComponent *StaminaComp{};
	
	UPROPERTY()
	TArray<FKnownHouse_MartensTuur> KnownHouses{};
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AWeapon>> KnownWeapons{};
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AFood>> KnownFoods{};
	
	UPROPERTY()
	TArray<FVector> KnownMedkits{};
	
	[[nodiscard]]
	TWeakObjectPtr<AWeapon> GetClosestWeapon() const;
	
	[[nodiscard]]
	FKnownHouse_MartensTuur *GetClosestHouse(bool bAllowChecked = true);
	
	[[nodiscard]]
	TWeakObjectPtr<AFood> GetClosestFood() const;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
