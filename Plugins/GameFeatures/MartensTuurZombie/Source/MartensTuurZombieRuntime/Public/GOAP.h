#pragma once
#include <memory>
#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"

#include "AIController.h"
#include "GOAP.generated.h"

UENUM(BlueprintType)
enum class EGOAPFlags_Martens_Tuur : uint8
{
	None = 0 UMETA(Hidden),
	HasWeapon,
	HasFood,
	HasMedkit,
	
	SeesEnemy,
	SeesPurgeZone,
	
	HasFoundWeapon,
	HasFoundHouse,
	HasFoundFood,
	HasFoundMedkit,
	
	HasFoundWeaponWithMoreAmmo,
	HasFreeInventorySlots,
	
	IsTired,
	IsFullHealth,
	KnowsUncheckedHouse,
	KnowsUncheckedPotentialNeighborLocations,
	Max UMETA(Hidden)
};

using EGOAPFlags_Value = uint32;

void LogFlags(EGOAPFlags_Value Flags);

USTRUCT(BlueprintType)
struct FGOAPState_Martens_Tuur
{
	GENERATED_BODY()
	
	EGOAPFlags_Value Flags;
	
	float Health{};
	float Stamina{};
	
	struct
	{
		int WeaponsNum{};
		int HousesNum{};
		int UncheckedHousesNum{};
		int FoodNum{};
		int MedkitsNum{};
		int EnemiesNum{};
		int PotentialNeighborLocationNum{};
	} AwareOf;
	
	struct
	{
		bool Food{};
		bool Weapon{};
		bool Medkit{};
		bool FreeSlots{};
	} InventoryContains;
	
	bool UpdateFlags();
};

USTRUCT(BlueprintType)
struct FCondition final
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category="GOAP")
	EGOAPFlags_Martens_Tuur StateKey{};
	
	UPROPERTY(EditDefaultsOnly, Category="GOAP")
	bool DesiredValue{};
	
	[[nodiscard]]
	bool DoesWorldStateConform(EGOAPFlags_Value State) const
	{
		auto const Bit = 1u << static_cast<uint8>(StateKey);
		
		if (DesiredValue)
			return (State & Bit) == Bit;
		
		return (State & Bit) == 0;
	}
};

UENUM(BlueprintType)
enum class EPriorityModifierValue : uint8
{
	OneMinusHealthPercentage,
	OneMinusStaminaPercentage,
	DistanceClosestFood,
	DistanceClosestMedkit,
	DistanceClosestWeapon,
};

UENUM(BlueprintType)
enum class EPriorityModifierType : uint8
{
	Additive,
	IncreaseScaleBy,
	AddExponentialValue,
	MultiplyModifierByConstant,
};

USTRUCT(BlueprintType)
struct FPriorityModifier
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	EPriorityModifierType Scaling{EPriorityModifierType::Additive};
	
	UPROPERTY(EditDefaultsOnly)
	EPriorityModifierValue Value;
	
	UPROPERTY(EditDefaultsOnly)
	float MultiplyBy{1.f};
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
	
	UPROPERTY(EditDefaultsOnly, Category="GOAP")
	float BasePriority{1.f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GOAP")
	TArray<FPriorityModifier> PriorityModifiers{};
	
	[[nodiscard]]
	float GetPriority(UHealthComponent *HealthComponent, UStaminaComponent *StaminaComponent) const;
	
	[[nodiscard]]
	// The lower the return value, the closer we adhere to the desired world state (i.e. the conditions).
	// The higher, the less we adhere, so the less content we are.
	// 0 to 1, 1 being least content
	// TODO: probably the heuristic score in A*?
	float GetDiscontentmentScore(EGOAPFlags_Value State) const;
	
	[[nodiscard]]
	bool IsSatisfied(EGOAPFlags_Value State) const;
};

USTRUCT(BlueprintType)
struct FEffect final
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GOAP")
	EGOAPFlags_Martens_Tuur StateKey{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GOAP")
	bool Value{};
	
	void Apply(EGOAPFlags_Value &State) const
	{
		auto const Bit = 1u << static_cast<uint8>(StateKey);
		
		if (Value)
			State |= Bit;
		else
			State &= ~Bit;
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
	
	UFUNCTION(BlueprintNativeEvent)
	void Begin(UObject *WorldContextObject, AAIController *Controller);
	
	UFUNCTION(BlueprintNativeEvent)
	EGOAPExecutorResult ExecutorTick(UObject *WorldContextObject, AAIController *Controller);
	
	UFUNCTION()
	virtual void OnFinish() {};
	
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
	EGOAPFlags_Value SimulateApplication(EGOAPFlags_Value Current) const;
	
	[[nodiscard]]
	bool CanExecute(EGOAPFlags_Value State) const;
	
	[[nodiscard]]
	bool HasAchievedEffects(EGOAPFlags_Value State) const;
	
	[[nodiscard]]
	UGOAPActionExecutor *GetAssociatedExecutorFromActor(AActor const* Actor) const;
};
