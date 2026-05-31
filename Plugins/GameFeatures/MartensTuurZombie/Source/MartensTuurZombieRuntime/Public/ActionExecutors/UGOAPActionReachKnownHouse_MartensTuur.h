#pragma once
#include "GOAP.h"
#include "StudentPerceptor_MartensTuur.h"
#include "UGOAPActionReachKnownHouse_MartensTuur.generated.h"

UCLASS()
class UGOAPActionReachKnownHouse_MartensTuur : public UGOAPActionExecutor
{
	GENERATED_BODY()
	
	FKnownHouse_MartensTuur *House{};
	
	UPROPERTY()
	AAIController* CachedController;
	
	UPROPERTY()
	UStudentPerceptor_MartensTuur *StudentPerceptor;
	
public:
	virtual void Begin_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual EGOAPExecutorResult ExecutorTick_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual void OnFinish() override;
};

