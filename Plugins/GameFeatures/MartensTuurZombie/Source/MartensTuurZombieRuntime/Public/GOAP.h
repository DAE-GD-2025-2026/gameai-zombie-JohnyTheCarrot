#pragma once

enum class EDesiredStateValue : uint8
{
	Indifferent,
	False,
	True,
};

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
		return HasWeapon == Other.HasWeapon;
	}
};

using FDesiredState = FState<EDesiredStateValue>;

struct FGoapBlackboard final
{
	int Health{};
	TOptional<float> DistanceToEnemy{};
};

using GoapPriority = float;
constexpr GoapPriority GoapUnnecessary{0.f};

class FGoal
{
public:
	virtual ~FGoal() = default;
	
	[[nodiscard]]
	virtual FDesiredState GetDesiredState() const = 0;
	
	[[nodiscard]]
	bool ArePreconditionsMet(FState<> const &State) const;
	
	[[nodiscard]]
	virtual GoapPriority MeasurePriority(FState<> const &State, FGoapBlackboard const &Blackboard) const = 0;
};

class FGoalKillEnemy final : public FGoal
{
public:
	virtual GoapPriority MeasurePriority(FState<> const &State, FGoapBlackboard const& Blackboard) const override;
	[[nodiscard]] virtual FDesiredState GetDesiredState() const override;
};

class FGoalGetWeapon final : public FGoal
{
public:
	[[nodiscard]] virtual GoapPriority
	MeasurePriority(const FState<>& State, const FGoapBlackboard& Blackboard) const override;
	[[nodiscard]] virtual FDesiredState GetDesiredState() const override;
};