#include "GOAP.h"

float FGoal::GetDiscontentmentScore(FWorldState const& State) const
{
	float Contentment = 0.f;
	for (auto const &[StateKey, Value] : Conditions)
	{
		if (State.Get(StateKey) == Value)
			Contentment += 1.f;
	}
	
	auto const NumConditions = static_cast<float>(Conditions.Num());
	return 1.f - Contentment / NumConditions;
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
