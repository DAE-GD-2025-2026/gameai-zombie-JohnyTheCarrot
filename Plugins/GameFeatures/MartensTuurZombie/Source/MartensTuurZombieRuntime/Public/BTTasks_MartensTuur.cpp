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

UBTCombat_MartensTuur::UBTCombat_MartensTuur()
{
	NodeName = TEXT("Combat");
	bNotifyTick = true;
}

FName const CombatKeepDistance{"Keep Distance"};

EBTNodeResult::Type UBTCombat_MartensTuur::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AgentBehavior == nullptr)
	{
		Pawn = Cast<ASurvivorPawn>(OwnerComp.GetAIOwner()->GetPawn());
		AgentBehavior = Pawn->GetComponentByClass<USurvivorAgentBehavior_MartensTuur>();
		check(AgentBehavior != nullptr);
	}
	if (Seek == nullptr)
	{
		Seek = NewObject<USteeringBehavior_Seek_MartensTuur>();
		Flee = NewObject<USteeringBehavior_Flee_MartensTuur>();
		Face = NewObject<USteeringBehavior_Face_MartensTuur>();
	}
	
	return EBTNodeResult::InProgress;
}

void UBTCombat_MartensTuur::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	check(AgentBehavior != nullptr);
	
	FVector2D FinalDir{};
	float TotalWeight{0.f};
	TWeakObjectPtr<ABaseZombie> ClosestZombie;
	float ClosestZombieDist{100000000000000.f};
	
	auto const &Zombies = AgentBehavior->GetKnownZombies();
	if (Zombies.IsEmpty())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
	for (TWeakObjectPtr<ABaseZombie> const& ZombiePtr : Zombies)
	{
		if (!ZombiePtr.IsValid())
			continue;

		ABaseZombie* Zombie = ZombiePtr.Get();

		FSteeringBehaviorTarget_MartensTuur Target{};
		Target.TargetLocation = Get2DVec(Zombie->GetActorLocation());
		Target.Velocity = Get2DVec(Zombie->GetVelocity());
		
		auto const Dist = Zombie->GetHorizontalDistanceTo(Pawn);
		if (ClosestZombieDist > Dist)
		{
			ClosestZombieDist = Dist;
			ClosestZombie = Zombie;
		}
		if (Dist > 300.f)
		{
			auto const SeekOutput = Seek->CalculateOutput(DeltaSeconds, Target, Pawn);
			FinalDir += SeekOutput.Direction;
			TotalWeight += 1.f;
		} else
		{
			auto const FleeOutput = Flee->CalculateOutput(DeltaSeconds, Target, Pawn);
			FinalDir += FleeOutput.Direction;
			TotalWeight += 1.f;
		}
	}
	
	FVector const ResultMovement{Get3DVec(FinalDir / TotalWeight)};
	
	if (!ResultMovement.IsNearlyZero())
	{
		Pawn->AddMovementInput(ResultMovement);
	}
	
	FSteeringBehaviorTarget_MartensTuur Target{};
	Target.TargetLocation = Get2DVec(ClosestZombie->GetActorLocation());
	AgentBehavior->TickSteeringBehavior(Face, Target, DeltaSeconds, 1.f);
	
	SecondsSinceShot += DeltaSeconds;
	if (Face->IsDone() && SecondsSinceShot >= 0.5f)
	{
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(Pawn);
		Params.AddIgnoredActor(ClosestZombie.Get());
		
		// obstacle check
		FHitResult HitResult{}; 
		bool bObstacleInTheWay = GetWorld()->LineTraceSingleByChannel(HitResult, Pawn->GetActorLocation(), ClosestZombie->GetActorLocation(), 
			ECC_Pawn, Params);
		
		if (!bObstacleInTheWay)
		{
			SecondsSinceShot = 0.f;
			AgentBehavior->Shoot();
		}
	}
}
