// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GOAP.h"
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

USTRUCT()
struct FKnownItem
{
	GENERATED_BODY()
	
	EItemType Type;
	FVector Location;
	
	[[nodiscard]]
	bool operator==(FKnownItem const &Other) const
	{
		return Location == Other.Location && Type == Other.Type;
	}
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MARTENSTUURZOMBIERUNTIME_API UStudentPerceptor : public UActorComponent
{
	GENERATED_BODY()
	
	EGOAPState SurvivorState;
	
	void RefreshSurvivorState();

public:
	// Sets default values for this component's properties
	UStudentPerceptor();
	
	UPROPERTY()
	TArray<FKnownHouse> KnownHouses{};
	
	UPROPERTY()
	TArray<FKnownItem> KnownItems{};
	
	[[nodiscard]]
	FKnownItem const *GetClosestWeapon() const;
	
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
