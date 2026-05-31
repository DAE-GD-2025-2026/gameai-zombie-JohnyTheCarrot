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
#include "Items/Medkit.h"
#include "Items/Weapon.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"
#include "Zombies/BaseZombie.h"
#include "StudentPerceptor_MartensTuur.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MARTENSTUURZOMBIERUNTIME_API UStudentPerceptor_MartensTuur : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStudentPerceptor_MartensTuur();
	
	void RefreshSurvivorState();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UGoapGraph *GoapComp{};
	
	UPROPERTY()
	UHealthComponent *HealthComp{};
	
	UPROPERTY()
	UStaminaComponent *StaminaComp{};
	
	UPROPERTY()
	UInventoryComponent *InventoryComp{};
	
	UPROPERTY()
	TMap<TWeakObjectPtr<ABaseZombie>, FVector> LastSeenZombiePos{};
	
	UPROPERTY()
	TArray<FKnownHouse_MartensTuur> CheckedHouses{};
	
	UPROPERTY()
	TArray<FKnownHouse_MartensTuur> UncheckedHouses{};
	
	UPROPERTY()
	// Mostly slightly outside the walls of houses we already know of, to try to find neighbors in villages
	TArray<FVector> PlacesToCheckForHouse{};
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AWeapon>> KnownWeapons{};
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AFood>> KnownFoods{};
	
	UPROPERTY()
	TArray<TWeakObjectPtr<AMedkit>> KnownMedkits{};
	
	[[nodiscard]]
	TWeakObjectPtr<AWeapon> GetClosestWeapon() const;
	
	[[nodiscard]]
	TWeakObjectPtr<AMedkit> GetClosestMedkit() const;
	
	[[nodiscard]]
	FKnownHouse_MartensTuur *GetClosestHouse();
	
	[[nodiscard]]
	TWeakObjectPtr<AFood> GetClosestFood() const;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
