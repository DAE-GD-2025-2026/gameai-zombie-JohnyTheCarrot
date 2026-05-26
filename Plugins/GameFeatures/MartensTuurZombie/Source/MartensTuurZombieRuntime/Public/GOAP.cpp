#include "GOAP.h"

float FGoal::GetDiscontentmentScore(FWorldState const& State) const
{
	float Discontentment = 0.f;
	for (auto const &[StateKey, Value] : Conditions)
	{
		if (State.Get(StateKey) != Value)
			Discontentment += 1.f;
	}
	
	return Discontentment;
}

bool FGoal::IsSatisfied(FWorldState const& State) const
{
	for (auto const &[StateKey, Value] : Conditions)
	{
		if (State.Get(StateKey) != Value)
			return false;
	}
	
	return true;
}

FWorldState UGOAPActionAsset::SimulateApplication(FWorldState const& Current) const
{
	FWorldState Result{Current};
	
	for (auto const &Effect : Effects)
	{
		Effect.Apply(Result);
	}
	
	return Result;
}

bool UGOAPActionAsset::CanExecute(FWorldState const& State) const
{
	for (auto const [ConditionKey, ConditionValue] : Preconditions)
	{
		if (State.Get(ConditionKey) != ConditionValue) return false;
	}
	
	return true;
}
