#include "GOAPPlanning.h"
#include <utility>
#include <queue>

struct FNode final
{
	TObjectPtr<UGOAPActionAsset> Action;
	
	EGOAPFlags_Martens_Tuur State;
	
	float CostFromStart{0.f};
	float HeuristicCost;
	
	FNode(TObjectPtr<UGOAPActionAsset> ActionIn, EGOAPFlags_Martens_Tuur StateIn, float CostIn, UGoal const &Goal)
		: Action{ActionIn}
		, State{Action->SimulateApplication(StateIn)}
		, CostFromStart{CostIn}
		, HeuristicCost{Goal.GetDiscontentmentScore(State)} {}
	
	FNode(EGOAPFlags_Martens_Tuur StartState, UGoal const &Goal)
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

void UGoapGraph::NextAction()
{
	if (CurrentPlan.IsValidIndex(CurrentActionIndex + 1))
	{
		if (auto const CurrentAction = GetCurrentAction())
			CurrentAction->GetAssociatedExecutorFromActor(GetOwner())->OnFinish();
		
		++CurrentActionIndex;
		
		auto CurrentExecutor = GetCurrentAction()->GetAssociatedExecutorFromActor(GetOwner());
		
		UE_LOG(LogTemp, Warning, TEXT("Begin action '%s'..."), *GetCurrentAction()->Name.ToString());
		auto const Controller = Cast<APawn>(GetOwner())->GetController();
		auto const AIController = Cast<AAIController>(Controller);
		CurrentExecutor->Begin(AIController, AIController);
		
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Plan completed!"));
	
	CurrentGoal = {};
	ActivateHighestPriorityGoal();
}

UGoapGraph::UGoapGraph()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UGoapGraph::InitializeGoap()
{
	HealthComp = GetOwner()->GetComponentByClass<UHealthComponent>();
	check(HealthComp);
	
	StaminaComp = GetOwner()->GetComponentByClass<UStaminaComponent>();
	check(StaminaComp);
	
	for (auto const &ActionAsset : AvailableActions)
	{
		UE_LOG(LogTemp, Warning, TEXT("Registring action %s executor"), *ActionAsset->Name.ToString())
		GetOwner()->AddComponentByClass(ActionAsset->ExecutorClass, false, FTransform::Identity, false);
	}
	ActivateHighestPriorityGoal();
}

UGoapGraph::GoapPlan UGoapGraph::Plan(FGOAPState_Martens_Tuur StartState, UGoal *Goal) const
{
	UE_LOG(LogTemp, Warning, TEXT("start of planning"));
	check(Goal != nullptr);
	FNode const StartNode{
		StartState.Flags,
		*Goal
	};
	std::priority_queue<FNode, std::vector<FNode>, std::greater<FNode>> OpenQueue{};
	OpenQueue.emplace(StartNode);
	
	struct FCameFrom final
	{
		EGOAPFlags_Martens_Tuur PreviousState;
		UGOAPActionAsset* Action;
	};
	
	std::unordered_map<EGOAPFlags_Martens_Tuur, FCameFrom> CameFromAction;
	std::unordered_map<EGOAPFlags_Martens_Tuur, float> GScores;
	GScores[StartState.Flags] = 0.f;
	
	UE_LOG(LogTemp, Warning, TEXT("Before start of while"));
	while (!OpenQueue.empty())
	{
		UE_LOG(LogTemp, Warning, TEXT("Start of while"));
		auto const Current = OpenQueue.top();
		OpenQueue.pop();
		
		// if (ClosedSet.contains(Current.State))
		// 	continue;
		
		UE_LOG(LogTemp, Warning, TEXT("Check if goal satisfied"));
		if (Goal->IsSatisfied(Current.State))
		{
			UE_LOG(LogTemp, Warning, TEXT("goal satisfied"));
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
			
			UE_LOG(LogTemp, Warning, TEXT("For the goal \"%s\", the plan is as follows:"), *Goal->Name.ToString());
			for (auto const &ActionAsset : Plan)
			{
				UE_LOG(LogTemp, Warning, TEXT("- %s"), *ActionAsset->Name.ToString());
			}
			
			UE_LOG(LogTemp, Warning, TEXT("... Profit, probably"));
			
			return Plan;
		}
		
		UE_LOG(LogTemp, Warning, TEXT("Checking %i available actions"), AvailableActions.Num());
		for (auto const &Action : AvailableActions)
		{
			if (!Action->CanExecute(Current.State))
			{
				UE_LOG(LogTemp, Warning, TEXT("Can't execute %s"), *Action->Name.ToString());
				continue;
			}
			
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

void UGoapGraph::ActivatePlan(TArray<UGOAPActionAsset*> const& Plan)
{
	CurrentPlan = std::move(Plan);
	CurrentActionIndex = -1;
	
	// next of -1 is 0, i.e. plan[0]
	NextAction();
}

void UGoapGraph::ActivateHighestPriorityGoal()
{
	TWeakObjectPtr<UGoal> HighestPriorityGoal{nullptr};
	float HighestPriorityFound = -1.f;
	
	int32 DebugKey = 15;
	for (auto const &Goal : Goals)
	{
		auto GoalPriority = Goal->GetPriority(HealthComp, StaminaComp);
		
		if (Goal->IsSatisfied(State.Flags)) GoalPriority = 0.f;
		GEngine->AddOnScreenDebugMessage(DebugKey, 1.f, FColor::Green, 
		FString::Printf(TEXT("goal \"%s\", prio %f"), *Goal->Name.ToString(), GoalPriority));
		
		++DebugKey;
		if (GoalPriority > HighestPriorityFound)
		{
			HighestPriorityFound = GoalPriority;
			HighestPriorityGoal = Goal.Get();
		}
	}
	
	if (CurrentGoal == HighestPriorityGoal)
		return;
	
	GEngine->AddOnScreenDebugMessage(DebugKey, 1.f, FColor::Green, 
	FString::Printf(TEXT("highest goal: \"%s\", prio %f"), *HighestPriorityGoal->Name.ToString(), HighestPriorityFound));
	auto const OldGoal = CurrentGoal;
	CurrentGoal = HighestPriorityGoal;
	
	ActivatePlan(Plan(State, CurrentGoal.Get()));
	
	if (OldGoal == nullptr) return;
	
	UE_LOG(LogTemp, Warning, TEXT("Changed goal from %s because goal %s became higher priority"), *OldGoal->Name.ToString(), *CurrentGoal->Name.ToString());
}

UGOAPActionAsset* UGoapGraph::GetCurrentAction() const
{
	// index 0 is invalid if the plan is empty
	if (!CurrentPlan.IsValidIndex(CurrentActionIndex)) return nullptr;
	
	return CurrentPlan[CurrentActionIndex];
}

void UGoapGraph::TickComponent(float DeltaTime, enum ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	// Yes, in tick, to account for changing conditions.
	ActivateHighestPriorityGoal();
	
	if (CurrentPlan.IsEmpty()) return;
	
	auto const *CurrentAction = GetCurrentAction();
	if (!CurrentAction)
	{
		UE_LOG(LogTemp, Warning, TEXT("No current action"));
		return;
	}
	
	auto const CurrentExecutor = CurrentAction->GetAssociatedExecutorFromActor(GetOwner());
	
	check(CurrentExecutor != nullptr);
	switch (CurrentExecutor->Status)
	{
	case EGOAPExecutorResult::Busy:
		{
			auto const Controller = Cast<APawn>(GetOwner())->GetController();
			auto const AIController = Cast<AAIController>(Controller);
			
			CurrentExecutor->Status = CurrentExecutor->ExecutorTick(AIController, AIController);
			if (CurrentAction->HasAchievedEffects(State.Flags))
				CurrentExecutor->Status = EGOAPExecutorResult::Success;
		}
		break;
	case EGOAPExecutorResult::Success:
		UE_LOG(LogTemp, Warning, TEXT("Action success"));
		// TODO: Not every action executor inherently guarantees fulfillment of an effect.
		// Finding a weapon requires reaching and checking a house, but a house doesn't necessarily contain a weapon.
		NextAction();
		break;
	case EGOAPExecutorResult::Failure:
		{
			UE_LOG(LogTemp, Warning, TEXT("Action failure, replanning..."));
			auto const NewPlan = Plan(State, CurrentGoal.Get());
			ActivatePlan(NewPlan);
			break;
		}
	}
}
