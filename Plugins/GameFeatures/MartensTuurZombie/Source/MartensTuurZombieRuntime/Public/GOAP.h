#pragma once
#include <memory>

#include "AIController.h"

enum class EGOAPState : uint8
{
	HasWeapon = 1 << 0u,
	HasFood = 1 << 1u,
	SeesEnemy = 1 << 2u,
};

class FWorldState final
{
	using Underlying = 
	std::underlying_type_t<EGOAPState>;
	Underlying State{0u};
	
public:
	[[nodiscard]]
	bool Get(EGOAPState Key) const
	{
		return (State & static_cast<Underlying>(Key)) != 0u;
	}
	
	void Set(EGOAPState Key, bool Value)
	{
		auto const Bit = static_cast<Underlying>(Key);
		
		if (Value)
			State |= Bit;
		else
			State &= ~Bit;
	}
};

USTRUCT(BlueprintType)
struct FCondition final
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	EGOAPState StateKey{};
	
	UPROPERTY(EditDefaultsOnly)
	bool DesiredValue{};
	
	[[nodiscard]]
	bool DoesWorldStateConform(FWorldState const &State) const
	{
		return State.Get(StateKey) == DesiredValue;
	}
};

USTRUCT(BlueprintType)
struct FGoal final
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FCondition> Conditions;
	
	[[nodiscard]]
	// The lower the return value, the closer we adhere to the desired world state (i.e. the conditions).
	// The higher, the less we adhere, so the less content we are.
	// 0 to 1, 1 being least content
	// TODO: probably the heuristic score in A*?
	float GetDiscontentmentScore(FWorldState const &State) const;
	
	[[nodiscard]]
	bool IsSatisfied(FWorldState const &State) const;
};

USTRUCT(BlueprintType)
struct FEffect final
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	EGOAPState StateKey{};
	
	UPROPERTY(EditDefaultsOnly)
	bool Value{};
	
	void Apply(FWorldState &State) const
	{
		State.Set(StateKey, Value);
	}
};

UCLASS(Blueprintable, Abstract)
class UGOAPActionExecutor : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void Begin(AAIController *Controller);
	
	UFUNCTION(BlueprintNativeEvent)
	void Tick(float DeltaTime);
	
	UFUNCTION(BlueprintNativeEvent)
	void Abort();
};

UCLASS(BlueprintType)
class UGOAPActionAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly)
	FName Name;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FCondition> Preconditions;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FEffect> Effects;
	
	UPROPERTY(EditDefaultsOnly)
	float BaseCost = 1.f;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGOAPActionExecutor> ExecutorClass;
	
	[[nodiscard]]
	FWorldState SimulateApplication(FWorldState const &Current) const;
	
	[[nodiscard]]
	bool CanExecute(FWorldState const &State) const;
};
