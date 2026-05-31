#include "UGOAPActionFetchItemByType_MartensTuur.h"

#include "NavigationSystem.h"
#include "AI/NavigationSystemBase.h"
#include "Navigation/PathFollowingComponent.h"

void UGOAPActionFetchItemByType_MartensTuur::Begin_Implementation(UObject* WorldContextObject, AAIController* Controller)
{
	Super::Begin_Implementation(WorldContextObject, Controller);
	
	CachedPerceptor = GetOwner()->GetComponentByClass<UStudentPerceptor>();
	check(CachedPerceptor != nullptr);
	
	CachedInventory = GetOwner()->GetComponentByClass<UInventoryComponent>();
	check(CachedInventory != nullptr);
	
	auto* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	check(NavSys);
	
	SelectTarget();
	check(Target.Get());
	
	auto const MoveResult = Controller->MoveToActor(Target.Get(), 10.f);
	
	Status = EGOAPExecutorResult::Busy;
	
	if (MoveResult == EPathFollowingRequestResult::Type::Failed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't reach item location, failing."));
		Finish(EGOAPExecutorResult::Failure);
	}
}

EGOAPExecutorResult UGOAPActionFetchItemByType_MartensTuur::ExecutorTick_Implementation(UObject* WorldContextObject,
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
			UE_LOG(LogTemp, Warning, TEXT("Couldn't pick up item because no free inventory slot"));
			return EGOAPExecutorResult::Failure;
		}
		
		if (CachedInventory->GrabItem(FreeSlot, Target.Get()))
		{
			RemoveFromKnown();
			return EGOAPExecutorResult::Success;
		}
		
		return EGOAPExecutorResult::Failure;
	}
	
	return EGOAPExecutorResult::Busy;
}

void UGOAPActionFetchWeapon_MartensTuur::SelectTarget()
{
	Target = CachedPerceptor->GetClosestWeapon();
}

void UGOAPActionFetchWeapon_MartensTuur::RemoveFromKnown()
{
	CachedPerceptor->KnownWeapons.Remove(Cast<AWeapon>(Target.Get()));
}

void UGOAPActionFetchFood_MartensTuur::SelectTarget()
{
	Target = CachedPerceptor->GetClosestFood();
}

void UGOAPActionFetchFood_MartensTuur::RemoveFromKnown()
{
	CachedPerceptor->KnownFoods.Remove(Cast<AFood>(Target.Get()));
}

