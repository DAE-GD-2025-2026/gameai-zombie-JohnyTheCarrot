#include "GOAP.h"

#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"

bool FGOAPState_Martens_Tuur::UpdateFlags()
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
	SetFlag(EGOAPFlags_Martens_Tuur::KnowsUncheckedHouse, AwareOf.UncheckedHousesNum > 0);
	SetFlag(EGOAPFlags_Martens_Tuur::SeesEnemy, AwareOf.EnemiesNum > 0);
	SetFlag(EGOAPFlags_Martens_Tuur::IsTired, Stamina < TirednessStaminaPercentageThreshold);
	SetFlag(EGOAPFlags_Martens_Tuur::HasFood, InventoryContains.Food);
	SetFlag(EGOAPFlags_Martens_Tuur::HasWeapon, InventoryContains.Weapon);
	
	return IsFlagsDirty;
}

float UGoal::GetPriority(UHealthComponent *HealthComponent, UStaminaComponent *StaminaComponent) const
{
	float ModifierValue = BasePriority;
	
	for (auto const &Modifier : PriorityModifiers)
	{
		float Value = 0.f;
		switch (Modifier.Value)
		{
		case EPriorityModifierValue::OneMinusHealthPercentage:
			Value = 1.f - static_cast<float>(HealthComponent->GetHealth()) / static_cast<float>(HealthComponent->GetMaxHealth());
			break;
		case EPriorityModifierValue::OneMinusStaminaPercentage:
			Value = 1.f - StaminaComponent->GetCurrentStamina() / StaminaComponent->GetMaxStamina();
			break;
		}
		
		switch (Modifier.Scaling)
		{
		case EPriorityModifierType::Additive:
			ModifierValue += Value;
			break;
		case EPriorityModifierType::IncreaseScaleBy:
			ModifierValue *= 1.f + Value;
			break;
		case EPriorityModifierType::AddExponentialValue:
			ModifierValue = FMath::Pow(ModifierValue, 1.f + Value);
			break;
		}
		
		ModifierValue *= Modifier.MultiplyBy;
	}
	
	return ModifierValue;
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

UGOAPActionExecutor* UGOAPActionAsset::GetAssociatedExecutorFromActor(AActor const* Actor) const
{
	return Cast<UGOAPActionExecutor>(Actor->GetComponentByClass(ExecutorClass));
}
