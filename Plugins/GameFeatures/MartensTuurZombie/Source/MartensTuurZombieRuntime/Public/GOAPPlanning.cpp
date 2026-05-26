#include "GOAPPlanning.h"
#include <utility>
#include <queue>
#include <unordered_set>

namespace GOAP
{
	struct FNode final
	{
		TObjectPtr<UGOAPActionAsset> Action;
		
		FWorldState State;
		
		float CostFromStart{0.f};
		float HeuristicCost;
		
		FNode(TObjectPtr<UGOAPActionAsset> ActionIn, FWorldState const &StateIn, float CostIn, FGoal const &Goal)
			: Action{ActionIn}
			, State{Action->SimulateApplication(StateIn)}
			, CostFromStart{CostIn}
			, HeuristicCost{Goal.GetDiscontentmentScore(State)} {}
		
		FNode(FWorldState const &StartState, FGoal const &Goal)
			: Action{nullptr}
			, State{StartState}
			, HeuristicCost{Goal.GetDiscontentmentScore(State)}
		{}
		
		[[nodiscard]]
		TArray<FCondition> const &GetConditions() const
		{
			if (Action == nullptr)
			{
				// fix dangling ref
				static const TArray<FCondition> EmptyConditions;
				// start node
				return EmptyConditions;
			}
			
			return Action->Preconditions;
		}
		
		[[nodiscard]]
		float GetCost() const
		{
			return CostFromStart + HeuristicCost;
		}
		
		auto operator<=>(const FNode& Other) const
		{
			return GetCost() <=> Other.GetCost();
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

	UGoapGraph::UGoapGraph(TArray<TObjectPtr<UGOAPActionAsset>> const& Actions)
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

	UGoapGraph::GoapPlan UGoapGraph::Plan(FWorldState const& StartState, FGoal const& Goal) const
	{
		FNode const StartNode{
			StartState,
			Goal
		};
		std::priority_queue<FNode, std::vector<FNode>, std::greater<FNode>> OpenQueue{};
		OpenQueue.emplace(StartNode);
		
		struct FCameFrom final
		{
			FWorldState PreviousState;
			UGOAPActionAsset* Action;
		};
		
		std::unordered_map<FWorldState, FCameFrom> CameFromAction;
		std::unordered_map<FWorldState, float> GScores;
		GScores[StartState] = 0.f;
		
		while (!OpenQueue.empty())
		{
			auto const Current = OpenQueue.top();
			OpenQueue.pop();
			
			// if (ClosedSet.contains(Current.State))
			// 	continue;
			
			if (Goal.IsSatisfied(Current.State))
			{
				// We are done, we have reached the goal!
				GoapPlan Plan;
				
				auto ReconstructCurrent{Current.State};
				while (CameFromAction.contains(ReconstructCurrent))
				{
					auto const &Step = CameFromAction[ReconstructCurrent];
					Plan.Add(Step.Action);
					
					ReconstructCurrent = Step.PreviousState;
				}
				
				Algo::Reverse(Plan);
				
				return Plan;
			}
			
			for (auto const Action : AvailableActions)
			{
				if (!Action->CanExecute(Current.State)) continue;
				
				auto const TentativeCost = Current.CostFromStart + Action->BaseCost;
				FNode Child{
					Action,
					Current.State,
					TentativeCost,
					Goal,
				};
				
				if (GScores.contains(Child.State) && TentativeCost >= GScores[Child.State]) continue;
				
				OpenQueue.emplace(Child);
				
				CameFromAction[Child.State] = FCameFrom{
					.PreviousState = Current.State,
					.Action = Action,
				};
				GScores[Child.State] = TentativeCost;
			}
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Empty plan returned!"));
		return {};
	}
}
