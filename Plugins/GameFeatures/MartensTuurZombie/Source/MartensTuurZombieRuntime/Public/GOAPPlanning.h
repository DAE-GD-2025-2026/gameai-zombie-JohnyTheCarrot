#pragma once
#include "GOAP.h"

#include "GOAPPlanning.generated.h"

using GoapGraphNode = TObjectPtr<UGOAPActionAsset>;

UCLASS()
class MARTENSTUURZOMBIERUNTIME_API UGoapGraph final : public UActorComponent
{
public:
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	TArray<TObjectPtr<UGOAPActionAsset>> AvailableActions;
	
	using GoapPlan = TArray<TObjectPtr<UGOAPActionAsset>>;
	
	[[nodiscard]]
	GoapPlan Plan(FWorldState const &StartState, FGoal const &Goal) const;
};