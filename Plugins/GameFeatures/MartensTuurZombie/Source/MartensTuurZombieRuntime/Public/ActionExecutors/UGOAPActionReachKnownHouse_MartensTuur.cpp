#include "UGOAPActionReachKnownHouse_MartensTuur.h"

#include "Navigation/PathFollowingComponent.h"


void UGOAPActionReachKnownHouse_MartensTuur::Begin_Implementation(UObject* WorldContextObject,
	AAIController* Controller)
{
	UGOAPActionExecutor::Begin_Implementation(WorldContextObject, Controller);
	CachedController = Controller;
	check(CachedController);
	
	StudentPerceptor = GetOwner()->GetComponentByClass<UStudentPerceptor>();
	check(StudentPerceptor != nullptr);
	
	auto const ClosestHouse = StudentPerceptor->GetClosestHouse();
	if (ClosestHouse == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("House to reach is null"));
		Finish(EGOAPExecutorResult::Failure);
		return;
	}
	
	House = ClosestHouse;
	
	Controller->StopMovement();
	auto const MoveResult = Controller->MoveToLocation(House->Bounds.Origin, 10.f);
	if (MoveResult == EPathFollowingRequestResult::Type::Failed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't reach house location, failing."));
		Finish(EGOAPExecutorResult::Failure);
	}
}

EGOAPExecutorResult UGOAPActionReachKnownHouse_MartensTuur::ExecutorTick_Implementation(UObject* WorldContextObject,
	AAIController* Controller)
{
	constexpr float AcceptanceRadius{100.f};
	if (FVector::DistSquared(GetOwner()->GetActorLocation(), House->Bounds.Origin) <= AcceptanceRadius * AcceptanceRadius)
	{
		return EGOAPExecutorResult::Success;
	}
	
	return EGOAPExecutorResult::Busy;
}

void UGOAPActionReachKnownHouse_MartensTuur::OnFinish()
{
	auto const CurrentPos = GetOwner()->GetActorLocation();
	UE_LOG(LogTemp, Warning, TEXT("Maybe marking house as checked"));
	
	// it is possible House was never changed, if we failed before we picked a new house target
	if (!House) return;
	
	// check if inside house, if so, mark haschecked. do this because we may have found our item on the way, which wouldn't make the house checked
	if (FMath::Abs(CurrentPos.X - House->Bounds.Origin.X) >= House->Bounds.Extent.X)
		return;
	
	if (FMath::Abs(CurrentPos.Y - House->Bounds.Origin.Y) >= House->Bounds.Extent.Y)
		return;
	
	UE_LOG(LogTemp, Warning, TEXT("Marking house as checked"));
	auto const HouseDeref = *House;
	StudentPerceptor->UncheckedHouses.Remove(HouseDeref);
	StudentPerceptor->CheckedHouses.Add(HouseDeref);
	StudentPerceptor->RefreshSurvivorState();
	House = nullptr;
}
