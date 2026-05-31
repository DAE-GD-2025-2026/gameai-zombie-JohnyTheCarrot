#pragma once
#include "GOAP.h"
#include "StudentPerceptor.h"
#include "UGOAPActionReachKnownHouse_MartensTuur.generated.h"

UCLASS()
class UGOAPActionReachKnownHouse_MartensTuur : public UGOAPActionExecutor
{
	GENERATED_BODY()
	
	UPROPERTY()
	FKnownHouse_MartensTuur House{};
	
	UPROPERTY()
	AAIController* CachedController;
	
	UPROPERTY()
	UStudentPerceptor *StudentPerceptor;
	
public:
	virtual void Begin_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual EGOAPExecutorResult ExecutorTick_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
};

