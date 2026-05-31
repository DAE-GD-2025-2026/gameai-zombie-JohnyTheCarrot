#include "GOAP.h"

#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"

void LogFlags(EGOAPFlags_Value Flags)
{
	for (auto I = 0; I < static_cast<uint8>(EGOAPFlags_Martens_Tuur::Max); ++I)
	{
		FString Name =
			StaticEnum<EGOAPFlags_Martens_Tuur>()->GetNameStringByValue(
				I
			);
		auto const Bit = 1u << I;
		FString IsSetStr = (Flags & Bit) == Bit ? "True" : "False";
		UE_LOG(LogTemp, Warning, TEXT("%s: %s"), *Name, *IsSetStr);
	}
}

bool FGOAPState_Martens_Tuur::UpdateFlags()
{
	bool IsFlagsDirty = false;
	
	auto const SetFlag = [this, &IsFlagsDirty](EGOAPFlags_Martens_Tuur Flag, bool Value)
	{
		auto const Bit = 1u << static_cast<uint8>(Flag);
		
		auto const CurrentValue = (Flags & Bit) == Bit;
		if (CurrentValue != Value) IsFlagsDirty = true;
		
		if (Value)
			Flags |= Bit;
		else
			Flags &= ~Bit;
	};
	
	// State Stamina: CurrentStamina / MaxStamina
	constexpr float TirednessStaminaPercentageThreshold = 0.4f;
	
	SetFlag(EGOAPFlags_Martens_Tuur::HasFoundWeapon, AwareOf.WeaponsNum > 0);
	SetFlag(EGOAPFlags_Martens_Tuur::HasFoundHouse, AwareOf.HousesNum > 0);
	SetFlag(EGOAPFlags_Martens_Tuur::HasFoundFood, AwareOf.FoodNum > 0);
	SetFlag(EGOAPFlags_Martens_Tuur::HasFoundMedkit, AwareOf.MedkitsNum > 0);
	SetFlag(EGOAPFlags_Martens_Tuur::KnowsUncheckedHouse, AwareOf.UncheckedHousesNum > 0);
	SetFlag(EGOAPFlags_Martens_Tuur::SeesEnemy, AwareOf.EnemiesNum > 0);
	SetFlag(EGOAPFlags_Martens_Tuur::IsTired, Stamina < TirednessStaminaPercentageThreshold);
	SetFlag(EGOAPFlags_Martens_Tuur::IsFullHealth, Health > 0.95f);
	SetFlag(EGOAPFlags_Martens_Tuur::HasFood, InventoryContains.Food);
	SetFlag(EGOAPFlags_Martens_Tuur::HasWeapon, InventoryContains.Weapon);
	SetFlag(EGOAPFlags_Martens_Tuur::HasMedkit, InventoryContains.Medkit);
	SetFlag(EGOAPFlags_Martens_Tuur::HasFreeInventorySlots, InventoryContains.FreeSlots);
	SetFlag(EGOAPFlags_Martens_Tuur::KnowsUncheckedPotentialNeighborLocations, AwareOf.PotentialNeighborLocationNum);
	
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

float UGoal::GetDiscontentmentScore(EGOAPFlags_Value State) const
{
	float Discontentment = 0.f;
	for (auto const &[StateKey, Value] : Conditions)
	{
		auto const Bit = 1u << static_cast<uint8>(StateKey);
		if ((State & Bit) == Bit)
			Discontentment += 1.f;
	}
	
	return Discontentment;
}

bool UGoal::IsSatisfied(EGOAPFlags_Value State) const
{
	uint32 Required = 0;
	uint32 Forbidden = 0;

	for (auto const& [StateKey, Value] : Conditions)
	{
		const uint32 Bit = 1u << static_cast<uint8>(StateKey);

		if (Value)
			Required |= Bit;
		else
			Forbidden |= Bit;
	}
	
	return (State & Required) == Required
		&& (State & Forbidden) == 0;
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

EGOAPFlags_Value UGOAPActionAsset::SimulateApplication(EGOAPFlags_Value Current) const
{
	auto Result{Current};
	
	for (auto const &Effect : Effects)
	{
		Effect.Apply(Result);
	}
	
	return Result;
}

bool UGOAPActionAsset::CanExecute(EGOAPFlags_Value State) const
{
	for (auto const [ConditionKey, ConditionValue] : Preconditions)
	{
		FString Name =
			StaticEnum<EGOAPFlags_Martens_Tuur>()->GetNameStringByValue(
				static_cast<int64>(ConditionKey)
			);
		auto const Bit = 1u << static_cast<uint8>(ConditionKey);
		auto const HasCondition = (State & Bit) == Bit;
		if (HasCondition != ConditionValue) return false;
	}
	
	return true;
}

bool UGOAPActionAsset::HasAchievedEffects(EGOAPFlags_Value State) const
{
	uint32 Required = 0;
	uint32 Forbidden = 0;

	for (auto const& [StateKey, Value] : Effects)
	{
		const uint32 Bit = 1u << static_cast<uint8>(StateKey);

		if (Value)
			Required |= Bit;
		else
			Forbidden |= Bit;
	}
	
	return (State & Required) == Required
		&& (State & Forbidden) == 0;
}

UGOAPActionExecutor* UGOAPActionAsset::GetAssociatedExecutorFromActor(AActor const* Actor) const
{
	return Cast<UGOAPActionExecutor>(Actor->GetComponentByClass(ExecutorClass));
}
