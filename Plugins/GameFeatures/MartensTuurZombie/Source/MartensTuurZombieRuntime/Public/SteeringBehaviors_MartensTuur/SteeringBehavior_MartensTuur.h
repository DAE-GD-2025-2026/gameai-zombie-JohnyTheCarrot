#pragma once
#include "Survivor/SurvivorPawn.h"
#include "SteeringBehavior_MartensTuur.generated.h"

USTRUCT(BlueprintType)
struct FSteeringBehaviorTarget_MartensTuur
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D TargetLocation{FVector2D::ZeroVector};
	
	UPROPERTY()
	FVector2D Velocity{FVector2D::ZeroVector};
	
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
	
	TOptional<UE::Math::TRotator<double>> FacingTowards{NullOpt};
	
	UPROPERTY(BlueprintReadOnly)
	float BlendWeight{1.f};
	
	UPROPERTY(BlueprintReadOnly)
	bool FaceDirection{false};
};

UCLASS(BlueprintType)
class USteeringBehavior_MartensTuur : public UObject
{
	GENERATED_BODY()
	
	UPROPERTY()
	bool bIsDone{false};

protected:
	virtual void OnReset() {};
	
public:
	using UObject::UObject;
	
	virtual ~USteeringBehavior_MartensTuur() override = default;
	
	void Finish()
	{
		bIsDone = true;
	}
	
	void SetNotDone()
	{
		bIsDone = false;
	}
	
	void Reset()
	{
		bIsDone = false;
		OnReset();
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
	float DoneAtDistance{10.f};
	
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
	
	TOptional<int> CurrentVecIdx{NullOpt};
	
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
class USteeringBehavior_Flee_MartensTuur : public USteeringBehavior_Seek_MartensTuur
{
	GENERATED_BODY()

public:
	virtual FSteeringOutput_MartensTuur CalculateOutput(float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent) override;
};

USTRUCT(BlueprintType)
struct FWeightedBehavior_MartensTuur final
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FName Key;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Weight;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<USteeringBehavior_MartensTuur> Behavior;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEnabled{true};
};

UCLASS(BlueprintType)
class USteeringBehavior_Blended_MartensTuur : public USteeringBehavior_MartensTuur
{
	GENERATED_BODY()

	TArray<FWeightedBehavior_MartensTuur> Behaviors;
	
	TOptional<int> FinishAuthorityIdx{NullOpt};
	
protected:
	virtual void OnReset() override;
	
public:
	UFUNCTION(BlueprintCallable)
	TArray<FWeightedBehavior_MartensTuur> const &GetBehaviors() const;
	
	[[nodiscard]]
	FWeightedBehavior_MartensTuur* FindBehaviorByKey(FName Key);
	
	virtual bool CheckIfDone(FSteeringOutput_MartensTuur const& Output, float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, AActor const* Agent) const override;
	
	/**
	 * @param NewBehavior The behavior to add.
	 * @param bIsFinishAuthority Whether this Behavior is the one who determines whether the BlendedSteering behavior is done.
	 * @return False if duplicate key, true if inserted
	 */
	UFUNCTION(BlueprintCallable)
	bool AddBehavior(FWeightedBehavior_MartensTuur NewBehavior, bool bIsFinishAuthority = false);
	
	void SetWeight(FName Key, float NewWeight);
	
	template<typename TBehavior>
	FWeightedBehavior_MartensTuur &GetOrAddBehavior(FName Key, float BlendWeight = 1.f)
	{
		if (auto *Existing = FindBehaviorByKey(Key))
			return *Existing;
		
		FWeightedBehavior_MartensTuur Behavior{};
		Behavior.Key = Key;
		Behavior.Weight = BlendWeight;
		Behavior.Behavior = NewObject<TBehavior>();
		check(AddBehavior(Behavior, false));
		
		auto const NewlyAdded = FindBehaviorByKey(Key);
		check(NewlyAdded != nullptr);
		return *NewlyAdded;
	}
	
	virtual FSteeringOutput_MartensTuur CalculateOutput(float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent) override;
};

UCLASS(BlueprintType)
class USteeringBehavior_Face_MartensTuur : public USteeringBehavior_MartensTuur
{
	GENERATED_BODY()

public:
	virtual bool CheckIfDone(FSteeringOutput_MartensTuur const& Output, float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, AActor const* Agent) const override;
	
	virtual FSteeringOutput_MartensTuur CalculateOutput(float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent) override;
};

UCLASS(BlueprintType)
class USteeringBehavior_TurnAround_MartensTuur : public USteeringBehavior_MartensTuur
{
	GENERATED_BODY()

public:
	TOptional<FVector> InitiallyFacingDirection{NullOpt};
	UE::Math::TRotator<double> TargetRotator{};
	
	virtual void OnReset() override
	{
		InitiallyFacingDirection.Reset();
	}
	
	virtual FSteeringOutput_MartensTuur CalculateOutput(float DeltaT, FSteeringBehaviorTarget_MartensTuur const& Target, ASurvivorPawn const* Agent) override;
};
