#pragma once
#include "GOAP.h"
#include "StudentPerceptor.h"
#include "Common/InventoryComponent.h"
#include "Items/Weapon.h"
#include "UGOAPActionFetchWeapon_MartensTuur.generated.h"

UCLASS()
class UGOAPActionFetchWeapon_MartensTuur : public UGOAPActionExecutor
{
	GENERATED_BODY()
	
	UPROPERTY()
	TWeakObjectPtr<AWeapon> Target{};
	
	UPROPERTY()
	UStudentPerceptor *CachedPerceptor{};
	
	UPROPERTY()
	UInventoryComponent* CachedInventory{};
	
public:
	virtual void Begin_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual EGOAPExecutorResult ExecutorTick_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
};
