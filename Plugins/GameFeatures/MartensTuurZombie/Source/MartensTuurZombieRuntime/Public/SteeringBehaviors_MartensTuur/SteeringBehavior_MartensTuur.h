#pragma once
#include "Survivor/SurvivorPawn.h"
#include "SteeringBehavior_MartensTuur.generated.h"

USTRUCT(BlueprintType)
struct FSteeringBehaviorTarget_MartensTuur
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D TargetLocation{FVector2D::ZeroVector};
	
	bool operator==(FSteeringBehaviorTarget_MartensTuur const &Other) const
	{
		return TargetLocation == Other.TargetLocation;
	}
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
	float SpeedScale{1.f};
	
	UPROPERTY(BlueprintReadOnly)
	bool FaceDirection{false};
};

UCLASS(BlueprintType)
class USteeringBehavior_MartensTuur : public UObject
{
	GENERATED_BODY()
	
	UPROPERTY()
	bool bIsDone{false};

public:
	using UObject::UObject;
	
	virtual ~USteeringBehavior_MartensTuur() override = default;
	
	void Finish()
	{
		bIsDone = true;
	}
	
	void Reset()
	{
		bIsDone = false;
	}
	
	UFUNCTION(BlueprintCallable)
	bool IsDone() const
	{
		return bIsDone;
	}
	
	[[nodiscard]]
	virtual bool CheckIfDone(FSteeringOutput_MartensTuur const &Output, float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, AActor const* Agent) const
	{
		return false;
	}
	
	UFUNCTION(BlueprintCallable)
	virtual FSteeringOutput_MartensTuur CalculateOutput(
		float DeltaT,
		FSteeringBehaviorTarget_MartensTuur const &Target,
		ASurvivorPawn const* Agent
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DoneAtDistance{50.f};
	
	virtual bool CheckIfDone(FSteeringOutput_MartensTuur const &Output, float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, AActor const* Agent) const override;
	
	virtual FSteeringOutput_MartensTuur CalculateOutput(float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent) override;
};

UCLASS(BlueprintType)
class USteeringBehavior_Arrive_MartensTuur : public USteeringBehavior_Seek_MartensTuur
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SlowAtDistance{200.f};
	
	virtual FSteeringOutput_MartensTuur CalculateOutput(float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent) override;
};

UCLASS(BlueprintType)
class USteeringBehavior_FollowPath_MartensTuur : public USteeringBehavior_Seek_MartensTuur
{
	GENERATED_BODY()
	
	UPROPERTY()
	FSteeringBehaviorTarget_MartensTuur LastTarget{};
	
	UPROPERTY()
	TArray<FVector> CurrentPath{};
	
	FVector *CurrentVec{};
	
public:
	virtual FSteeringOutput_MartensTuur CalculateOutput(float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent) override;
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
	
	virtual FSteeringOutput_MartensTuur CalculateOutput(float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent) override;
};

UCLASS(BlueprintType)
class USteeringBehavior_Flee_MartensTuur : public USteeringBehavior_MartensTuur
{
	GENERATED_BODY()

public:
	virtual FSteeringOutput_MartensTuur CalculateOutput(float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent) override;
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
