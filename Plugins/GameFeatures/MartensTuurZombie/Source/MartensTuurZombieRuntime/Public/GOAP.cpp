#include "GOAP.h"

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

bool FGoal::ArePreconditionsMet(FState<> const& State) const
{
	return State == GetDesiredState();
}

GoapPriority FGoalKillEnemy::MeasurePriority(FState<> const &State, FGoapBlackboard const& Blackboard) const
{
	// TODO: increase priority if enemy is aggro compared to unaware?
	return 1.f;
}

FDesiredState FGoalKillEnemy::GetDesiredState() const
{
	return {
		.HasWeapon = EDesiredStateValue::True,
		.EnemyIsVisible = EDesiredStateValue::True,
	};
}

GoapPriority FGoalGetWeapon::MeasurePriority(const FState<>& State, const FGoapBlackboard& Blackboard) const
{
	return 1.f;
}

FDesiredState FGoalGetWeapon::GetDesiredState() const
{
	return {
		.HasWeapon = EDesiredStateValue::False,
		.HasFoundWeapon = EDesiredStateValue::True,
	};
}
