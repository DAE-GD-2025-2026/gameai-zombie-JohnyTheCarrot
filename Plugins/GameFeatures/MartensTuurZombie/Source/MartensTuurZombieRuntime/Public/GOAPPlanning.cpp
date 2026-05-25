#include "GOAPPlanning.h"
#include <span>
#include <queue>
#include <unordered_set>

namespace GOAP
{
	struct FNode final
	{
		TObjectPtr<UGOAPActionAsset> Action;
		
		float CostFromStart = 0.f;
		float HeuristicCost;
		
		[[nodiscard]]
		float GetCost() const
		{
			return CostFromStart + HeuristicCost;
		}
		
		auto operator<=>(const FNode& other) const
		{
			return HeuristicCost <=> other.HeuristicCost;
		}
	};
	
	TArray<TObjectPtr<UGOAPActionExecutor>> Plan(FWorldState const& StartState,
		TArray<UGOAPActionAsset*> const& Actions)
	{
		TArray<TObjectPtr<UGOAPActionExecutor>> result;
		std::priority_queue<FNode> OpenList;
	}

	float FGoapGraphConnection::GetCost(FWorldState const& /*State*/) const
	{
		// TODO: the cost can depend on things like distance, which you'd prob get from the executor
		return To->BaseCost;
	}

	FGOAPGraph::FGOAPGraph(TArray<TObjectPtr<UGOAPActionAsset>> const& Actions)
		: DesiredStateByQualifyingActions{[&Actions] -> StateToActionsMap
		{
			StateToActionsMap Map{};
			
			for (auto Action : Actions)
			{
				for (auto const &[EffectKey, ResultingValue] : Action->Effects)
				{
					if (ResultingValue)
						Map.Add(EffectKey, Action);
				}
			}
			
			return Map;
		}()} {
	}

	FGOAPGraph::GoapPlan FGOAPGraph::Plan(FWorldState const& StartState, FGoal const& Goal) const
	{
		// Priority queue doesn't support random access, meaning I can't check whether a given element is already in it ...
		std::priority_queue<FNode, std::vector<FNode>, std::greater<FNode>> OpenQueue;
		// ... that's why we have an open set, which enables dupe checking.
		std::unordered_set<GoapGraphNode> OpenSet;
		
		std::unordered_set<GoapGraphNode> ClosedSet;
		
		std::unordered_map<GoapGraphNode, GoapGraphNode> CameFrom;
		std::unordered_map<GoapGraphNode, float> GScores;
		std::unordered_map<GoapGraphNode, float> FScores;
		
		FWorldState CurrentState{StartState};
		float CostSoFar = 0.f;
		
		auto const Heuristic = [&Goal, &CurrentState](GoapGraphNode const &Node)
		{
			return Goal.GetDiscontentmentScore(Node->SimulateApplication(CurrentState));
		};
		
		auto const AddToOpenList = [this, CostSoFar, &Heuristic, &OpenQueue, &Goal, &CurrentState](TArray<FCondition> const& Conditions)
		{
			for (auto const &Condition : Conditions)
			{
				TArray<TObjectPtr<UGOAPActionAsset>> QualifyingActions;
				DesiredStateByQualifyingActions.MultiFind(Condition.StateKey, QualifyingActions);
			
				for (auto const Action : QualifyingActions)
				{
					OpenQueue.emplace(
						Action,
						CostSoFar + Action->BaseCost,
						Heuristic(Action)
					);
				}
			}
		};
		
		auto const GetCostScore = [](std::unordered_map<GoapGraphNode, float> const &Scores, GoapGraphNode const& Node) -> float
		{
			if (auto const It = Scores.find(Node); It != Scores.end())
				return It->second;
			
			return std::numeric_limits<float>::max();
		};
		
		// get qualifying actions for goal, add them to OpenList
		AddToOpenList(Goal.Conditions);
		
		TArray<FCondition> TotalConditions{Goal.Conditions};
		while (!OpenQueue.empty())
		{
			auto const Current = OpenQueue.top();
			ClosedSet.emplace(Current.Action);
			
			auto const ResultingState = Current.Action->SimulateApplication(CurrentState);
			
			if (Goal.GetDiscontentmentScore(ResultingState) == 0.f)
			{
				// TODO: we are done, we have reached the goal!
				GoapPlan Plan;
				
				auto ReconstructCurrent{Current.Action};
				
				decltype(CameFrom)::iterator It;
				while ((It = CameFrom.find(ReconstructCurrent)) != CameFrom.end())
				{
					ReconstructCurrent = CameFrom[ReconstructCurrent];
					Plan.Add(ReconstructCurrent);
				}
				
				Algo::Reverse(Plan);
				
				return Plan;
			}
			
			OpenQueue.pop();
			
			auto const &ActionPreconditions = Current.Action->Preconditions;
			for (auto const ActionPrecondition : ActionPreconditions)
			{
				TArray<TObjectPtr<UGOAPActionAsset>> QualifyingActions; // Neighbors
				DesiredStateByQualifyingActions.MultiFind(ActionPrecondition.StateKey, QualifyingActions);
			
				for (auto const Action : QualifyingActions)
				{
					auto const TentativeGScore = Current.CostFromStart + Action->BaseCost;
					if (TentativeGScore >= GetCostScore(GScores, Action)) continue;
					
					CameFrom.insert_or_assign(Action, Current.Action);
					GScores.insert_or_assign(Action, TentativeGScore);
					FScores.insert_or_assign(Action, TentativeGScore + Heuristic(Action));
					
					ClosedSet.erase(Action);
					OpenQueue.emplace(
						Action,
						TentativeGScore,
						Heuristic(Action)
					);
				}
			}
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Empty plan returned!"));
		return {};
	}

	void UGOAPActionPlanner::BeginPlay()
	{
		Super::BeginPlay();
		
		Graph = FGOAPGraph{AvailableActions};
	}
}
