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

UCLASS(BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MARTENSTUURZOMBIERUNTIME_API UStudentPerceptor_MartensTuur : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStudentPerceptor_MartensTuur();
	
	void RefreshSurvivorState() {}
	
	UPROPERTY(BlueprintReadOnly)
	UHealthComponent *HealthComp{};
	
	UPROPERTY(BlueprintReadOnly)
	UStaminaComponent *StaminaComp{};
	
	UPROPERTY(BlueprintReadOnly)
	UInventoryComponent *InventoryComp{};
	
	UPROPERTY()
	TMap<TWeakObjectPtr<ABaseZombie>, FVector> LastSeenZombiePos{};
	
	UPROPERTY(BlueprintReadOnly)
	TArray<FKnownHouse_MartensTuur> CheckedHouses{};
	
	UPROPERTY(BlueprintReadOnly)
	TArray<AHouse*> UncheckedHouses{};
	
	UFUNCTION(BlueprintCallable)
	void MarkHouseChecked(AHouse *House);
	
	UPROPERTY(BlueprintReadOnly)
	// Mostly slightly outside the walls of houses we already know of, to try to find neighbors in villages
	TArray<FVector> PlacesToCheckForHouse{};
	
	UPROPERTY(BlueprintReadOnly)
	TArray<AWeapon*> KnownWeapons{};
	
	UPROPERTY(BlueprintReadOnly)
	TArray<AFood*> KnownFoods{};
	
	UPROPERTY(BlueprintReadOnly)
	TArray<AMedkit*> KnownMedkits{};
	
	[[nodiscard]]
	TWeakObjectPtr<AWeapon> GetClosestWeapon() const;
	
	[[nodiscard]]
	TWeakObjectPtr<AMedkit> GetClosestMedkit() const;
	
	UFUNCTION(BlueprintCallable)
	AHouse *GetClosestHouse();
	
	[[nodiscard]]
	TWeakObjectPtr<AFood> GetClosestFood() const;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
