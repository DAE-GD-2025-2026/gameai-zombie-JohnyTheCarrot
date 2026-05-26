#pragma once

#include <unordered_map>

#include "GOAP.h"

namespace GOAP
{
	[[nodiscard]]
	TArray<TObjectPtr<UGOAPActionExecutor>> Plan(
		FWorldState const &StartState,
		TArray<UGOAPActionAsset*> const &Actions
	);
	
	using GoapGraphNode = TObjectPtr<UGOAPActionAsset>;
	
	struct FGoapGraphConnection final
	{
		GoapGraphNode To;
		GoapGraphNode From;
		
		[[nodiscard]]
		float GetCost(FWorldState const &State) const;
	};
	
	UCLASS()
	class MARTENSTUURZOMBIERUNTIME_API UGoapGraph final : public UActorComponent
	{
		GENERATED_BODY()
		
		using StateToActionsMap = TMultiMap<EGOAPState, TObjectPtr<UGOAPActionAsset>>;
		StateToActionsMap DesiredStateByQualifyingActions;
		TArray<FGoapGraphConnection> Connections;
		
	public:
		UPROPERTY(EditDefaultsOnly)
		TArray<TObjectPtr<UGOAPActionAsset>> AvailableActions;
		
		explicit UGoapGraph(TArray<TObjectPtr<UGOAPActionAsset>> const &Actions);
		
		using GoapPlan = TArray<TObjectPtr<UGOAPActionAsset>>;
		
		[[nodiscard]]
		GoapPlan Plan(FWorldState const &StartState, FGoal const &Goal) const;
	};
}