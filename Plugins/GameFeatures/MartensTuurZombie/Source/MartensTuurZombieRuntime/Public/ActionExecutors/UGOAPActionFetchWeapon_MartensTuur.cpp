#include "UGOAPActionFetchWeapon_MartensTuur.h"

#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Navigation/PathFollowingComponent.h"

void UGOAPActionFetchWeapon_MartensTuur::Begin_Implementation(UObject* WorldContextObject, AAIController* Controller)
{
	Super::Begin_Implementation(WorldContextObject, Controller);
	
	CachedPerceptor = GetOwner()->GetComponentByClass<UStudentPerceptor>();
	check(CachedPerceptor != nullptr);
	
	CachedInventory = GetOwner()->GetComponentByClass<UInventoryComponent>();
	check(CachedInventory != nullptr);
	
	auto* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	check(NavSys);
	
	auto const ClosestWeapon = CachedPerceptor->GetClosestWeapon();
	check(ClosestWeapon.IsValid()); // we shouldn't fail this, because one of the preconditions is knowing where a weapon is.
	
	Target = ClosestWeapon;
	auto const MoveResult = Controller->MoveToActor(Target.Get(), 10.f);
	
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
	auto const PickupRange = CachedInventory->GetPickupRange();
	if (FVector::DistSquared(GetOwner()->GetActorLocation(), Target->GetActorLocation()) <= PickupRange * PickupRange)
	{
		auto FreeSlot = -1;
		auto const Inv = CachedInventory->GetInventory();
		for (int I = 0; I < Inv.Num(); ++I)
		{
			if (Inv[I] == nullptr) FreeSlot = I;
		}
		if (FreeSlot == -1)
		{
			UE_LOG(LogTemp, Warning, TEXT("Couldn't pick up weapon because no free inventory slot"));
			return EGOAPExecutorResult::Failure;
		}
		
		CachedInventory->GrabItem(FreeSlot, Target.Get());
		return EGOAPExecutorResult::Success;
	}
	
	return EGOAPExecutorResult::Busy;
}
