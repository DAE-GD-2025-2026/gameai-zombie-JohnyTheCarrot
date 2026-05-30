#include "UGOAPActionFetchWeapon_MartensTuur.h"

#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Navigation/PathFollowingComponent.h"

constexpr static float AcceptanceRadius{50.f};

void UGOAPActionFetchWeapon_MartensTuur::Begin_Implementation(UObject* WorldContextObject, AAIController* Controller)
{
	Super::Begin_Implementation(WorldContextObject, Controller);
	
	CachedPerceptor = GetOwner()->GetComponentByClass<UStudentPerceptor>();
	check(CachedPerceptor != nullptr);
	
	auto* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	check(NavSys);
	
	auto const *ClosestWeapon = CachedPerceptor->GetClosestWeapon();
	check(ClosestWeapon); // we shouldn't fail this, because one of the preconditions is knowing where a weapon is.
	
	CurrentDestination = *ClosestWeapon;
	auto const MoveResult = Controller->MoveToLocation(CurrentDestination, AcceptanceRadius);
	
	Status = EGOAPExecutorResult::Busy;
	
	if (MoveResult == EPathFollowingRequestResult::Type::Failed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't reach weapon location, failing."));
		Finish(EGOAPExecutorResult::Failure);
	}
}

EGOAPExecutorResult UGOAPActionFetchWeapon_MartensTuur::ExecutorTick_Implementation(UObject* WorldContextObject,
	AAIController* Controller)
{
	if (FVector::DistSquared(GetOwner()->GetActorLocation(), CurrentDestination * CurrentDestination) <= AcceptanceRadius)
	{
		return EGOAPExecutorResult::Success;
	}
	
	return EGOAPExecutorResult::Busy;
}
