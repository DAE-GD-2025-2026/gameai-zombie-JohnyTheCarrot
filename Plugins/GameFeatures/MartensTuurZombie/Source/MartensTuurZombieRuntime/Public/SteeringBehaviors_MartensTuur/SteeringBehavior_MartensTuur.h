#pragma once
#include "Survivor/SurvivorPawn.h"
#include "SteeringBehavior_MartensTuur.generated.h"

USTRUCT(BlueprintType)
struct FSteeringBehaviorTarget_MartensTuur
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D TargetLocation{FVector2D::ZeroVector};
};

[[nodiscard]]
FVector2D Get2DVec(FVector Vec);

[[nodiscard]]
FVector Get3DVec(FVector2D Vec);

USTRUCT(BlueprintType)
struct FSteeringOutput_MartensTuur
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FVector2D Direction{FVector::ZeroVector};
	
	UPROPERTY(BlueprintReadOnly)
	bool FaceDirection{false};
};

UCLASS(BlueprintType)
class USteeringBehavior_MartensTuur : public UObject
{
	GENERATED_BODY()
	
public:
	using UObject::UObject;
	
	virtual ~USteeringBehavior_MartensTuur() override = default;
	
	UFUNCTION(BlueprintCallable)
	virtual FSteeringOutput_MartensTuur CalculateOutput(
		float DeltaT,
		FSteeringBehaviorTarget_MartensTuur const &Target,
		AActor const* Agent
	)
	{
		return {};
	}
};

UCLASS(BlueprintType)
class USteeringBehavior_Seek_MartensTuur : public USteeringBehavior_MartensTuur
{
	GENERATED_BODY()
	
public:
	virtual FSteeringOutput_MartensTuur CalculateOutput(float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, AActor const* Agent) override;
};

UCLASS(BlueprintType)
class USteeringBehavior_Wander_MartensTuur : public USteeringBehavior_Seek_MartensTuur
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MinAngle{-45.f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxAngle{+45.f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Distance{200.f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Radius{150.f};
	
	virtual FSteeringOutput_MartensTuur CalculateOutput(float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, AActor const* Agent) override;
};

UCLASS(BlueprintType)
class USteeringBehavior_Flee_MartensTuur : public USteeringBehavior_MartensTuur
{
	GENERATED_BODY()

public:
	virtual FSteeringOutput_MartensTuur CalculateOutput(float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, AActor const* Agent) override;
};

USTRUCT(BlueprintType)
struct FWeightedBehavior final
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Weight;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<USteeringBehavior_MartensTuur> Behavior;
};

UCLASS(BlueprintType)
class UBlendedSteering : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName Name;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FWeightedBehavior> Behaviors;
};
