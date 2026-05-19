#include "GOAP.h"

void ApplyDesiredState(bool& Val, EDesiredStateValue Desired)
{
	switch (Desired)
	{
	case EDesiredStateValue::True:
		Val = true;
		break;
	case EDesiredStateValue::False:
		Val = false;
		break;
	case EDesiredStateValue::Indifferent:
		break;
	}
}

bool operator==(bool Value, EDesiredStateValue Desired)
{
	switch (Desired)
	{
	case EDesiredStateValue::True:
		return Value;
	case EDesiredStateValue::False:
		return !Value;
	case EDesiredStateValue::Indifferent:
		return true;
	}
	checkNoEntry();
}

bool FAction::ArePreconditionsMet(FState<> const& State) const
{
	return State == GetRequiredState();
}

GoapPriority FGoalIsSafe::MeasurePriority(FState<> const &State, FGoapBlackboard const& Blackboard) const
{
	// TODO: increase priority if enemy is aggro compared to unaware?
	return 1.f;
}

FDesiredState FGoalIsSafe::GetDesiredState() const
{
	return {
		.HasWeapon = EDesiredStateValue::True,
		.EnemyIsVisible = EDesiredStateValue::False,
	};
}

GoapPriority FGoalExplore::MeasurePriority(const FState<>& State, const FGoapBlackboard& Blackboard) const
{
	return 1.f;
}

FDesiredState FGoalExplore::GetDesiredState() const
{
	return {
		.HasWeapon = EDesiredStateValue::False,
		.HasFoundWeapon = EDesiredStateValue::True,
	};
}

float FActionFindWeapon::GetCost() const
{
	// TODO: cost
	return 1.f;
}

FDesiredState FActionFindWeapon::GetResultingState() const
{
	return {
		.HasWeapon = EDesiredStateValue::True
	};
}

FDesiredState FActionFindWeapon::GetRequiredState() const
{
	return {
		.HasWeapon =  EDesiredStateValue::False,
		.HasFoundWeapon = EDesiredStateValue::False,
	};
}
