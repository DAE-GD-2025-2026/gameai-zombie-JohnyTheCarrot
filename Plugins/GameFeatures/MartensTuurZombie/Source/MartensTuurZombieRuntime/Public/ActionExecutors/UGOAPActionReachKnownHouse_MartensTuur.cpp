#include "UGOAPActionReachKnownHouse_MartensTuur.h"

#include "Navigation/PathFollowingComponent.h"

constexpr static float AcceptanceRadius{30.f};

void UGOAPActionReachKnownHouse_MartensTuur::Begin_Implementation(UObject* WorldContextObject,
	AAIController* Controller)
{
	UGOAPActionExecutor::Begin_Implementation(WorldContextObject, Controller);
	CachedController = Controller;
	check(CachedController);
	
	StudentPerceptor = GetOwner()->GetComponentByClass<UStudentPerceptor>();
	check(StudentPerceptor != nullptr);
	
	auto const ClosestPos = StudentPerceptor->GetClosestHouse();
	check(ClosestPos);
	
	HouseLocation = *ClosestPos;
	
	auto const MoveResult = Controller->MoveToLocation(HouseLocation, AcceptanceRadius);
	if (MoveResult == EPathFollowingRequestResult::Type::Failed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't reach weapon location, failing."));
		Finish(EGOAPExecutorResult::Failure);
	}
}

EGOAPExecutorResult UGOAPActionReachKnownHouse_MartensTuur::ExecutorTick_Implementation(UObject* WorldContextObject,
	AAIController* Controller)
{
	if (FVector::DistSquared(GetOwner()->GetActorLocation(), HouseLocation) <= AcceptanceRadius * AcceptanceRadius)
	{
		return EGOAPExecutorResult::Success;
	}
	
	return EGOAPExecutorResult::Busy;
}
