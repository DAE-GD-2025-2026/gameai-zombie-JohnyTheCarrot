#include "GOAP.h"

float UGoal::GetDiscontentmentScore(EGOAPState State) const
{
	float Discontentment = 0.f;
	for (auto const &[StateKey, Value] : Conditions)
	{
		if (EnumHasAllFlags(State, StateKey) == Value)
			Discontentment += 1.f;
	}
	
	return Discontentment;
}

bool UGoal::IsSatisfied(EGOAPState State) const
{
	EGOAPState FinalDesiredState{};
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

void UGOAPActionExecutor::Abort_Implementation()
{
}

EGOAPState UGOAPActionAsset::SimulateApplication(EGOAPState Current) const
{
	auto Result{Current};
	
	for (auto const &Effect : Effects)
	{
		Effect.Apply(Result);
	}
	
	return Result;
}

bool UGOAPActionAsset::CanExecute(EGOAPState State) const
{
	UE_LOG(LogTemp, Warning, TEXT("Checking CanExecute..."));
	for (auto const [ConditionKey, ConditionValue] : Preconditions)
	{
		FString Name =
			StaticEnum<EGOAPState>()->GetNameStringByValue(
				static_cast<int64>(EGOAPState::HasWeapon)
			);
		auto const HasCondition = EnumHasAllFlags(State, ConditionKey);
		UE_LOG(LogTemp, Warning, TEXT("Expects %s to be %s, is %s."), *Name, *FString{ConditionValue ? "True" : "False"}, *FString{HasCondition ? "True" : "False"});
		if (EnumHasAllFlags(State, ConditionKey) != ConditionValue) return false;
	}
	
	return true;
}
