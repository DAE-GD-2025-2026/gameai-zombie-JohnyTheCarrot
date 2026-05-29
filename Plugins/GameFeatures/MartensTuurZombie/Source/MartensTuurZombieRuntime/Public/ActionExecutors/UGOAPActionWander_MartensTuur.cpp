#include "UGOAPActionWander_MartensTuur.h"

#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"

void UGOAPActionWander_MartensTuur::PickNewDestination()
{
	if (!CachedController) return;
	
	auto* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	check(NavSys);
	
	check(CachedController->GetPawn());
	auto const Origin = CachedController->GetPawn()->GetActorLocation();
	constexpr float Radius{500.f};
	
	FNavLocation ResultLocation;
	auto const PointFound = NavSys->GetRandomReachablePointInRadius(
		Origin,
		Radius,
		ResultLocation
	);
	
	if (!PointFound)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(
			this,
			&UGOAPActionWander_MartensTuur::PickNewDestination
		);
		
		return;
	}
	
	CurrentDestination = ResultLocation.Location;
	
	auto MoveResult =
		CachedController->MoveToLocation(CurrentDestination);

	if (MoveResult == EPathFollowingRequestResult::Type::Failed)
	{
		PickNewDestination();
	}
}

void UGOAPActionWander_MartensTuur::Begin_Implementation(UObject* WorldContextObject, AAIController* Controller)
{
	CachedController = Controller;
	PickNewDestination();
}

EGOAPExecutorResult UGOAPActionWander_MartensTuur::ExecutorTick_Implementation(UObject* WorldContextObject,
	AAIController* Controller)
{
	Super::ExecutorTick_Implementation(WorldContextObject, Controller);
	
	if (!CachedController) return EGOAPExecutorResult::Busy;
	
	auto const Pawn = Controller->GetPawn();
	check(Pawn);
	
	constexpr float AcceptableDistance{100.f};

	if (auto const Distance = FVector::Distance(Pawn->GetActorLocation(), CurrentDestination);
		Distance <= AcceptableDistance)
	{
		PickNewDestination();
	}
	
	return EGOAPExecutorResult::Busy;
}
