#pragma once
#include <memory>

#include "AIController.h"
#include "GOAP.generated.h"

UENUM(BlueprintType, meta=(Bitflags, BitmaskEnum="EGOAPState"))
enum class EGOAPState : uint8
{
	None = 0 UMETA(Hidden),
	HasWeapon = 1 << 0u,
	HasFood = 1 << 1u,
	SeesEnemy = 1 << 2u,
	// TODO: if we don't already know where one is, searching has an unknown cost.. what do
	HasFoundWeapon = 1 << 3u,
	HasFoundHouse = 1 << 4u,
};

ENUM_CLASS_FLAGS(EGOAPState);

USTRUCT(BlueprintType)
struct FCondition final
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="GOAP")
	EGOAPState StateKey{};
	
	UPROPERTY(EditDefaultsOnly, Category="GOAP")
	bool DesiredValue{};
	
	[[nodiscard]]
	bool DoesWorldStateConform(EGOAPState State) const
	{
		if (DesiredValue)
			return (State & StateKey) == StateKey;
		
		return (State & StateKey) == EGOAPState::None;
	}
};

UCLASS(BlueprintType)
class UGoal final : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GOAP")
	FName Name;
	
	// TODO: narrow down edit, read/write etc
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GOAP")
	TArray<FCondition> Conditions;
	
	[[nodiscard]]
	// The lower the return value, the closer we adhere to the desired world state (i.e. the conditions).
	// The higher, the less we adhere, so the less content we are.
	// 0 to 1, 1 being least content
	// TODO: probably the heuristic score in A*?
	float GetDiscontentmentScore(EGOAPState State) const;
	
	[[nodiscard]]
	bool IsSatisfied(EGOAPState State) const;
};

USTRUCT(BlueprintType)
struct FEffect final
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GOAP")
	EGOAPState StateKey{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GOAP")
	bool Value{};
	
	void Apply(EGOAPState &State) const
	{
		if (Value)
			State |= StateKey;
		else
			State &= ~StateKey;
	}
};

UENUM(BlueprintType)
enum class EGOAPExecutorResult : uint8
{
	Busy,
	Success,
	Failure,
};

UCLASS(Blueprintable)
class UGOAPActionExecutor : public UActorComponent
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	EGOAPExecutorResult Status{EGOAPExecutorResult::Busy};
	
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Begin(UObject *WorldContextObject, AAIController *Controller);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	EGOAPExecutorResult ExecutorTick(UObject *WorldContextObject, AAIController *Controller);
	
	UFUNCTION(BlueprintCallable)
	void Finish(EGOAPExecutorResult Result);
};

UCLASS(BlueprintType)
class UGOAPActionAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	FName Name;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FCondition> Preconditions;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FEffect> Effects;
	
	UPROPERTY(EditDefaultsOnly)
	float BaseCost = 1.f;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UGOAPActionExecutor> ExecutorClass;
	
	[[nodiscard]]
	EGOAPState SimulateApplication(EGOAPState Current) const;
	
	[[nodiscard]]
	bool CanExecute(EGOAPState State) const;
};
