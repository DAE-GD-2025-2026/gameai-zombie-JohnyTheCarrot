#include "GOAPPlanning.h"
#include <utility>
#include <queue>

struct FNode final
{
	TObjectPtr<UGOAPActionAsset> Action;
	
	EGOAPState State;
	
	float CostFromStart{0.f};
	float HeuristicCost;
	
	FNode(TObjectPtr<UGOAPActionAsset> ActionIn, EGOAPState StateIn, float CostIn, UGoal const &Goal)
		: Action{ActionIn}
		, State{Action->SimulateApplication(StateIn)}
		, CostFromStart{CostIn}
		, HeuristicCost{Goal.GetDiscontentmentScore(State)} {}
	
	FNode(EGOAPState StartState, UGoal const &Goal)
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

void UGoapGraph::InitializeGoap()
{
	for (auto const &ActionAsset : AvailableActions)
	{
		GetOwner()->AddComponentByClass(ActionAsset->ExecutorClass, false, FTransform::Identity, false);
	}
}

UGoapGraph::GoapPlan UGoapGraph::Plan(EGOAPState StartState, UGoal *Goal) const
{
	check(Goal != nullptr);
	FNode const StartNode{
		StartState,
		*Goal
	};
	std::priority_queue<FNode, std::vector<FNode>, std::greater<FNode>> OpenQueue{};
	OpenQueue.emplace(StartNode);
	
	struct FCameFrom final
	{
		EGOAPState PreviousState;
		UGOAPActionAsset* Action;
	};
	
	std::unordered_map<EGOAPState, FCameFrom> CameFromAction;
	std::unordered_map<EGOAPState, float> GScores;
	GScores[StartState] = 0.f;
	
	while (!OpenQueue.empty())
	{
		auto const Current = OpenQueue.top();
		OpenQueue.pop();
		
		// if (ClosedSet.contains(Current.State))
		// 	continue;
		
		if (Goal->IsSatisfied(Current.State))
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
		
		for (auto const &Action : AvailableActions)
		{
			if (!Action->CanExecute(Current.State)) continue;
			
			auto const TentativeCost = Current.CostFromStart + Action->BaseCost;
			FNode Child{
				Action,
				Current.State,
				TentativeCost,
				*Goal,
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
