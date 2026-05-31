#include "UGOAPActionScanHouseFor_MartensTuur.h"

#include "Navigation/PathFollowingComponent.h"


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
		return EGOAPExecutorResult::Success;
	
	constexpr float AcceptanceRadius{50.f};
	if (FVector::DistSquared(GetOwner()->GetActorLocation(), House.Bounds.Origin) <= AcceptanceRadius * AcceptanceRadius)
		return EGOAPExecutorResult::Failure;
	
	return EGOAPExecutorResult::Busy;
}

void UGOAPActionScanHouseFor_MartensTuur::OnFinish()
{
	auto const CurrentPos = GetOwner()->GetActorLocation();
	
	// check if inside house, if so, mark haschecked. do this because we may have found our item on the way, which wouldn't make the house checked
	if (FMath::Abs(CurrentPos.X - House.Bounds.Origin.X) >= House.Bounds.Extent.X)
		return;
	
	if (FMath::Abs(CurrentPos.Y - House.Bounds.Origin.Y) >= House.Bounds.Extent.Y)
		return;
	
	House.HasChecked = true;
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
