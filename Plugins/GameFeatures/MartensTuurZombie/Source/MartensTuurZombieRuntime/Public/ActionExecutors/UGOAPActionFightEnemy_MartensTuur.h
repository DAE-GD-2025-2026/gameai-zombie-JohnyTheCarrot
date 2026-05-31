#pragma once
#include "GOAP.h"
#include "StudentPerceptor.h"
#include "UGOAPActionFightEnemy_MartensTuur.generated.h"

UCLASS()
class UGOAPActionFightEnemy_MartensTuur : public UGOAPActionExecutor
{
	GENERATED_BODY()
	
	UPROPERTY()
	UStudentPerceptor *StudentPerceptor;
	
public:
	virtual void Begin_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual EGOAPExecutorResult ExecutorTick_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
};
