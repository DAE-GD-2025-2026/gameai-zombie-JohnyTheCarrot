#pragma once
#include "GOAP.h"

#include "GOAPPlanning.generated.h"

using GoapGraphNode = TObjectPtr<UGOAPActionAsset>;

UCLASS(ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class MARTENSTUURZOMBIERUNTIME_API UGoapGraph final : public UActorComponent
{
public:
	GENERATED_BODY()
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="GOAP")
	TArray<TObjectPtr<UGOAPActionAsset>> AvailableActions;
	
	using GoapPlan = TArray<TObjectPtr<UGOAPActionAsset>>;
	
	[[nodiscard]]
	GoapPlan Plan(FWorldState const &StartState, FGoal const &Goal) const;
};