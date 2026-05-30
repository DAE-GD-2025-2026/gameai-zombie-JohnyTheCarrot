#include "UGOAPActionReachKnownHouse_MartensTuur.h"

constexpr float AcceptanceRadius{30.f};

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
}

EGOAPExecutorResult UGOAPActionReachKnownHouse_MartensTuur::ExecutorTick_Implementation(UObject* WorldContextObject,
	AAIController* Controller)
{
	if (FVector::DistSquared(Controller->GetOwner()->GetActorLocation(), HouseLocation * HouseLocation) <= AcceptanceRadius)
	{
		return EGOAPExecutorResult::Success;
	}
	
	return EGOAPExecutorResult::Busy;
}
