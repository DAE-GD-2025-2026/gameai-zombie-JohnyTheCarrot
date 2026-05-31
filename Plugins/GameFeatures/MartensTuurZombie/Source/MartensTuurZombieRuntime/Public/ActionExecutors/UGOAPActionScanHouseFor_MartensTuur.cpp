#include "UGOAPActionScanHouseFor_MartensTuur.h"

#include "Navigation/PathFollowingComponent.h"

constexpr static float AcceptanceRadius{50.f};

void UGOAPActionScanHouseFor_MartensTuur::Begin_Implementation(UObject* WorldContextObject, AAIController* Controller)
{
	UGOAPActionExecutor::Begin_Implementation(WorldContextObject, Controller);
	
	CachedStudentPerceptor = GetOwner()->GetComponentByClass<UStudentPerceptor>();
	check(CachedStudentPerceptor != nullptr);
	
	auto const ClosestUncheckedHouse = CachedStudentPerceptor->GetClosestHouse(false);
	if (ClosestUncheckedHouse == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("No unchecked houses, failing."));
		Finish(EGOAPExecutorResult::Failure);
		return;
	}
	House = *ClosestUncheckedHouse;
	
	auto const MoveResult = Controller->MoveToLocation(House.Bounds.Origin, 10.f);
	if (MoveResult == EPathFollowingRequestResult::Type::Failed)
	{
		UE_LOG(LogTemp, Warning, TEXT("Couldn't reach house location, failing."));
		Finish(EGOAPExecutorResult::Failure);
	}
}

EGOAPExecutorResult UGOAPActionScanHouseFor_MartensTuur::ExecutorTick_Implementation(UObject* WorldContextObject,
	AAIController* Controller)
{
	if (IsDone())
	{
		House.HasChecked = true;
		return EGOAPExecutorResult::Success;
	}
	
	if (FVector::DistSquared(GetOwner()->GetActorLocation(), House.Bounds.Origin) <= AcceptanceRadius * AcceptanceRadius)
	{
		House.HasChecked = true;
		return EGOAPExecutorResult::Failure;
	}
	
	return EGOAPExecutorResult::Busy;
}

void UGoapActionScanHouseForWeapon_MartensTuur::Begin_Implementation(UObject* WorldContextObject,
	AAIController* Controller)
{
	Super::Begin_Implementation(WorldContextObject, Controller);
	NumKnownWeaponsAtStart = CachedStudentPerceptor->KnownWeapons.Num();
}

bool UGoapActionScanHouseForWeapon_MartensTuur::IsDone()
{
	return CachedStudentPerceptor->KnownWeapons.Num() > NumKnownWeaponsAtStart;
}

void UGoapActionScanHouseForFood_MartensTuur::Begin_Implementation(UObject* WorldContextObject,
	AAIController* Controller)
{
	Super::Begin_Implementation(WorldContextObject, Controller);
	NumKnownFoodsAtStart = CachedStudentPerceptor->KnownFoods.Num();
}

bool UGoapActionScanHouseForFood_MartensTuur::IsDone()
{
	return CachedStudentPerceptor->KnownFoods.Num() > NumKnownFoodsAtStart;
}
