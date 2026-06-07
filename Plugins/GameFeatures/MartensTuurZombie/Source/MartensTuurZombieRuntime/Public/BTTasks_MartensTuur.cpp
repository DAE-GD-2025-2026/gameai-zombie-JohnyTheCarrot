#include "BTTasks_MartensTuur.h"

#include "AIController.h"
#include "StudentPerceptor_MartensTuur.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Common/InventoryComponent.h"
#include "SteeringBehaviors_MartensTuur/SteeringBehavior_MartensTuur.h"
#include "Survivor/SurvivorPawn.h"

UBTMarkHouseExplored::UBTMarkHouseExplored()
{
	NodeName = TEXT("Mark house as explored");
}

EBTNodeResult::Type UBTMarkHouseExplored::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto *const Controller = OwnerComp.GetAIOwner();
	auto Pawn = Controller->GetPawn();
	auto *const AgentBehavior = Cast<ASurvivorPawn>(Pawn)->GetComponentByClass<USurvivorAgentBehavior_MartensTuur>();
	
	auto *Blackboard = Controller->GetBlackboardComponent();
	auto const House = Cast<AHouse>(Blackboard->GetValueAsObject(HouseKey.SelectedKeyName));
	AgentBehavior->MarkChecked(House);
	Blackboard->ClearValue(HouseKey.SelectedKeyName);
	
	return EBTNodeResult::Succeeded;
}

UBTFindNewWanderPos::UBTFindNewWanderPos()
{
	NodeName = TEXT("Find new wander position");
}

EBTNodeResult::Type UBTFindNewWanderPos::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto *const Controller = OwnerComp.GetAIOwner();
	auto Agent = Controller->GetPawn();
	
	auto const Angle = FMath::RandRange(MinAngle, MaxAngle);
	FVector2D const AngleVec{FMath::Cos(Angle), FMath::Sin(Angle)};
	
	FVector2D const TargetPos{Get2DVec(Agent->GetActorLocation()) + Get2DVec(Agent->GetActorForwardVector()) * Distance + AngleVec * Radius};
	UE_LOG(LogTemp, Warning, TEXT("new wander target: %f, %f"), TargetPos.X, TargetPos.Y)
	
	auto *Blackboard = Controller->GetBlackboardComponent();
	Blackboard->SetValueAsVector(WanderKey.SelectedKeyName, Get3DVec(TargetPos));
	
	return EBTNodeResult::Succeeded;
}

UBTGetItem::UBTGetItem()
{
	NodeName = TEXT("Get item");
}

EBTNodeResult::Type UBTGetItem::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	auto *const Controller = OwnerComp.GetAIOwner();
	auto *Blackboard = Controller->GetBlackboardComponent();
	auto *const Item = Cast<ABaseItem>(Blackboard->GetValueAsObject(ItemKey.SelectedKeyName));
	
	auto *const Agent = Cast<ASurvivorPawn>(Controller->GetPawn());
	auto *const AgentBehavior = Agent->GetComponentByClass<USurvivorAgentBehavior_MartensTuur>();
	auto const GrabResult = AgentBehavior->GrabItem(Item);
	
	if (!GrabResult) UE_LOG(LogTemp, Warning, TEXT("Couldn't grab item"));
	return GrabResult
		? EBTNodeResult::Succeeded
		: EBTNodeResult::Failed;
}
