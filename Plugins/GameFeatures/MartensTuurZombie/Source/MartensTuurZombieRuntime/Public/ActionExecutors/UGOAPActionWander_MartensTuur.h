#pragma once
#include "GOAP.h"
#include "UGOAPActionWander_MartensTuur.generated.h"

UCLASS()
class UGOAPActionWander_MartensTuur : public UGOAPActionExecutor
{
	GENERATED_BODY()
	
	UPROPERTY()
	FVector CurrentDestination{};
	
	UPROPERTY()
	AAIController* CachedController;
	
	void PickNewDestination();
	
public:
	virtual void Begin_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual EGOAPExecutorResult ExecutorTick_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
};
