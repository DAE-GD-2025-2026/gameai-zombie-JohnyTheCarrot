#include "GOAP.h"

void FGOAPState_Martens_Tuur::UpdateFlags()
{
	bool IsFlagsDirty = false;
	
	auto const SetFlag = [this, &IsFlagsDirty](EGOAPFlags_Martens_Tuur Flag, bool Value)
	{
		auto const CurrentValue = (Flags & Flag) == Flag;
		if (CurrentValue != Value) IsFlagsDirty = true;
		
		if (Value)
			EnumAddFlags(Flags, Flag);
		else
			EnumRemoveFlags(Flags, Flag);
	};
	
	// State Stamina: CurrentStamina / MaxStamina
	constexpr float TirednessStaminaPercentageThreshold = 0.2f;
	
	SetFlag(EGOAPFlags_Martens_Tuur::HasFoundWeapon, AwareOf.WeaponsNum > 0);
	SetFlag(EGOAPFlags_Martens_Tuur::HasFoundHouse, AwareOf.HousesNum > 0);
	SetFlag(EGOAPFlags_Martens_Tuur::SeesEnemy, AwareOf.EnemiesNum > 0);
	SetFlag(EGOAPFlags_Martens_Tuur::IsTired, Stamina < TirednessStaminaPercentageThreshold);
	
	if (IsFlagsDirty)
	{
		// TODO: flags changed, check if our plan can still work
	}
}

float UGoal::GetDiscontentmentScore(EGOAPFlags_Martens_Tuur State) const
{
	float Discontentment = 0.f;
	for (auto const &[StateKey, Value] : Conditions)
	{
		if (EnumHasAllFlags(State, StateKey) == Value)
			Discontentment += 1.f;
	}
	
	return Discontentment;
}

bool UGoal::IsSatisfied(EGOAPFlags_Martens_Tuur State) const
{
	EGOAPFlags_Martens_Tuur FinalDesiredState{};
	for (auto const &[StateKey, Value] : Conditions)
	{
		if (Value)
			EnumAddFlags(FinalDesiredState, StateKey);
		else
			EnumRemoveFlags(FinalDesiredState, StateKey);
	}
	
	return EnumHasAllFlags(State, FinalDesiredState);
}

void UGOAPActionExecutor::BeginPlay()
{
	Super::BeginPlay();
	UE_LOG(LogTemp, Warning, TEXT("attached UGOAPActionExecutor"));
}

void UGOAPActionExecutor::Finish(EGOAPExecutorResult Result)
{
	Status = Result;
}

EGOAPExecutorResult UGOAPActionExecutor::ExecutorTick_Implementation(UObject* WorldContextObject, AAIController* Controller)
{
	return EGOAPExecutorResult::Success;
}

void UGOAPActionExecutor::Begin_Implementation(UObject* WorldContextObject, AAIController* Controller)
{
}

EGOAPFlags_Martens_Tuur UGOAPActionAsset::SimulateApplication(EGOAPFlags_Martens_Tuur Current) const
{
	auto Result{Current};
	
	for (auto const &Effect : Effects)
	{
		Effect.Apply(Result);
	}
	
	return Result;
}

bool UGOAPActionAsset::CanExecute(EGOAPFlags_Martens_Tuur State) const
{
	UE_LOG(LogTemp, Warning, TEXT("Checking CanExecute..."));
	for (auto const [ConditionKey, ConditionValue] : Preconditions)
	{
		FString Name =
			StaticEnum<EGOAPFlags_Martens_Tuur>()->GetNameStringByValue(
				static_cast<int64>(ConditionKey)
			);
		auto const HasCondition = EnumHasAllFlags(State, ConditionKey);
		UE_LOG(LogTemp, Warning, TEXT("Expects %s to be %s, is %s."), *Name, *FString{ConditionValue ? "True" : "False"}, *FString{HasCondition ? "True" : "False"});
		if (EnumHasAllFlags(State, ConditionKey) != ConditionValue) return false;
	}
	
	return true;
}

bool UGOAPActionAsset::HasAchievedEffects(EGOAPFlags_Martens_Tuur State) const
{
	EGOAPFlags_Martens_Tuur FinalDesiredState{};
	for (auto const &[StateKey, Value] : Effects)
	{
		if (Value)
			EnumAddFlags(FinalDesiredState, StateKey);
		else
			EnumRemoveFlags(FinalDesiredState, StateKey);
	}
	
	return EnumHasAllFlags(State, FinalDesiredState);
}
