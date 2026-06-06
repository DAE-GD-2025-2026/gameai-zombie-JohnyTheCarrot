#include "BTTasks.h"

#include "AIController.h"
#include "StudentPerceptor_MartensTuur.h"
#include "BehaviorTree/BlackboardComponent.h"
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
	auto *const Perceptor = Cast<ASurvivorPawn>(Pawn)->GetComponentByClass<UStudentPerceptor_MartensTuur>();
	
	auto *Blackboard = Controller->GetBlackboardComponent();
	auto const House = Cast<AHouse>(Blackboard->GetValueAsObject(HouseKey.SelectedKeyName));
	Perceptor->MarkChecked(House);
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
	UE_LOG(LogTemp, Warning, TEXT("Target: %f, %f"), TargetPos.X, TargetPos.Y)
	
	auto *Blackboard = Controller->GetBlackboardComponent();
	Blackboard->SetValueAsVector(WanderKey.SelectedKeyName, Get3DVec(TargetPos));
	
	return EBTNodeResult::Succeeded;
}
