#pragma once
#include "GOAP.h"
#include "StudentPerceptor.h"
#include "Common/InventoryComponent.h"
#include "UGOAPActionFetchWeapon_MartensTuur.generated.h"

UCLASS()
class UGOAPActionFetchWeapon_MartensTuur : public UGOAPActionExecutor
{
	GENERATED_BODY()
	
	UPROPERTY()
	FVector CurrentDestination{};
	
	UPROPERTY()
	AAIController* CachedController{};
	
	UPROPERTY()
	UStudentPerceptor *CachedPerceptor{};
	
public:
	virtual void Begin_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual EGOAPExecutorResult ExecutorTick_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
};
