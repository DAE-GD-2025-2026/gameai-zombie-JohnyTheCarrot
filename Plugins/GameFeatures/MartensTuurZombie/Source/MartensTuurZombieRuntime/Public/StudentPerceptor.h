// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAP.h"
#include "GOAPPlanning.h"
#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"
#include "Components/ActorComponent.h"
#include "Items/ItemType.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISense_Damage.h"
#include "StudentPerceptor.generated.h"

USTRUCT()
struct FKnownHouse final
{
	GENERATED_BODY()
	
	FVector Location;
	
	[[nodiscard]]
	bool operator==(const FKnownHouse& Other) const
	{
		return Location == Other.Location;
	}
};

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
	TArray<FKnownHouse> KnownHouses{};
	
	UPROPERTY()
	TArray<FVector> KnownWeapons{};
	
	UPROPERTY()
	TArray<FVector> KnownFoods{};
	
	UPROPERTY()
	TArray<FVector> KnownMedkits{};
	
	[[nodiscard]]
	FVector const *GetClosestWeapon() const;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
