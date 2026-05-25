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
	
	class FGOAPGraph final
	{
		using StateToActionsMap = TMultiMap<EGOAPState, TObjectPtr<UGOAPActionAsset>>;
		StateToActionsMap DesiredStateByQualifyingActions;
		TArray<FGoapGraphConnection> Connections;
		
	public:
		explicit FGOAPGraph(TArray<TObjectPtr<UGOAPActionAsset>> const &Actions);
		
		using GoapPlan = TArray<TObjectPtr<UGOAPActionAsset>>;
		
		[[nodiscard]]
		GoapPlan Plan(FWorldState const &StartState, FGoal const &Goal) const;
	};
	
	UCLASS()
	class MARTENSTUURZOMBIERUNTIME_API UGOAPActionPlanner : public UActorComponent
	{
		GENERATED_BODY()
		
	public:
		UPROPERTY(EditDefaultsOnly)
		TArray<TObjectPtr<UGOAPActionAsset>> AvailableActions;
		
		FGOAPGraph Graph;
		
		virtual void BeginPlay() override;
	};
}