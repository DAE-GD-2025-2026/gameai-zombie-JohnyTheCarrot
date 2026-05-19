#pragma once
#include <vector>
#include <memory>

enum class EDesiredStateValue : uint8
{
	Indifferent,
	False,
	True,
};

void ApplyDesiredState(bool& Val, EDesiredStateValue Desired);

[[nodiscard]]
bool operator==(bool Value, EDesiredStateValue Desired);

[[nodiscard]]
inline bool operator!=(bool Value, EDesiredStateValue Desired)
{
	return !(Value == Desired);
}

[[nodiscard]]
inline bool operator==(EDesiredStateValue Desired, bool Value)
{
	return Value == Desired;
}

[[nodiscard]]
inline bool operator!=(EDesiredStateValue Desired, bool Value)
{
	return Value != Desired;
}

static_assert(EDesiredStateValue{} == EDesiredStateValue::Indifferent);

template<typename ValueType = bool>
struct FState final
{
	ValueType HasWeapon{};
	ValueType HasFoundWeapon{};
	ValueType EnemyIsVisible{};
	
	template<typename OtherValueType>
	[[nodiscard]]
	bool operator==(FState<OtherValueType> const &Other)
	{
		return HasWeapon == Other.HasWeapon
			&& HasFoundWeapon == Other.HasFoundWeapon
			&& EnemyIsVisible == Other.EnemyIsVisible
		;
	}
	
	template<typename OtherValueType>
	[[nodiscard]]
	FState<>& operator=(FState<OtherValueType> const &Other)
	{
		ApplyDesiredState(HasWeapon, Other.HasWeapon);
		ApplyDesiredState(HasFoundWeapon, Other.HasFoundWeapon);
		ApplyDesiredState(EnemyIsVisible, Other.EnemyIsVisible);
		
		return *this;
	}
};

using FDesiredState = FState<EDesiredStateValue>;

struct FGoapBlackboard final
{
	int Health{};
	TOptional<float> DistanceToEnemy{};
};

using GoapPriority = float;

class FGoal
{
public:
	virtual ~FGoal() = default;
	
	[[nodiscard]]
	virtual FDesiredState GetDesiredState() const = 0;
	
	[[nodiscard]]
	virtual GoapPriority MeasurePriority(FState<> const &State, FGoapBlackboard const &Blackboard) const = 0;
};

class FGoalIsSafe final : public FGoal
{
public:
	virtual GoapPriority MeasurePriority(FState<> const &State, FGoapBlackboard const& Blackboard) const override;
	[[nodiscard]] virtual FDesiredState GetDesiredState() const override;
};

class FGoalExplore final : public FGoal
{
public:
	[[nodiscard]] virtual GoapPriority
	MeasurePriority(const FState<>& State, const FGoapBlackboard& Blackboard) const override;
	[[nodiscard]] virtual FDesiredState GetDesiredState() const override;
};

class FAction 
{
public:
	virtual ~FAction() = default;
	
	[[nodiscard]]
	virtual FDesiredState GetRequiredState() const = 0;
	
	[[nodiscard]]
	virtual FDesiredState GetResultingState() const = 0;
	
	[[nodiscard]]
	bool ArePreconditionsMet(FState<> const &State) const;
	
	[[nodiscard]]
	virtual float GetCost() const = 0;
};

class FActionFindWeapon final : public FAction
{
public:
	[[nodiscard]] virtual FDesiredState GetRequiredState() const override;
	[[nodiscard]] virtual FDesiredState GetResultingState() const override;
	
	virtual float GetCost() const override;
};

class FGoalPlanner final
{
	std::vector<std::unique_ptr<FGoal>> Goals;
	
public:
	explicit FGoalPlanner(std::vector<std::unique_ptr<FGoal>> GoalsIn)
		: Goals{std::move(GoalsIn)}
	{}
};