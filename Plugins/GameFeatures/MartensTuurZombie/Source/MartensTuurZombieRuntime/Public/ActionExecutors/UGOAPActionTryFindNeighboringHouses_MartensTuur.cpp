#include "UGOAPActionTryFindNeighboringHouses_MartensTuur.h"

#include "StudentPerceptor.h"
#include "Navigation/PathFollowingComponent.h"

void UGOAPActionTryFindNeighboringHouses_MartensTuur::Begin_Implementation(UObject* WorldContextObject, AAIController* Controller)
{
	UGOAPActionExecutor::Begin_Implementation(WorldContextObject, Controller);
	
	CachedStudentPerceptor = GetOwner()->GetComponentByClass<UStudentPerceptor>();
	check(CachedStudentPerceptor != nullptr);
	
	CurrentDestination = CachedStudentPerceptor->PlacesToCheckForHouse.Top();
	NumHousesAtStart = CachedStudentPerceptor->UncheckedHouses.Num();
	
	auto const MoveResult = Controller->MoveToLocation(CurrentDestination, 10.f);
	if (MoveResult == EPathFollowingRequestResult::Type::Failed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't reach potential neighbor location, failing."));
		CachedStudentPerceptor->PlacesToCheckForHouse.Remove(CurrentDestination);
		Finish(EGOAPExecutorResult::Failure);
	}
}

EGOAPExecutorResult UGOAPActionTryFindNeighboringHouses_MartensTuur::ExecutorTick_Implementation(
	UObject* WorldContextObject, AAIController* Controller)
{
	constexpr float AcceptanceRadius{50.f};
	if (FVector::DistSquared(GetOwner()->GetActorLocation(), CurrentDestination) <= AcceptanceRadius * AcceptanceRadius)
	{
		CachedStudentPerceptor->PlacesToCheckForHouse.Remove(CurrentDestination);
		
		if (CachedStudentPerceptor->UncheckedHouses.Num() > NumHousesAtStart)
			return EGOAPExecutorResult::Success;
		
		return EGOAPExecutorResult::Failure;
	}
	
	return EGOAPExecutorResult::Busy;
}
