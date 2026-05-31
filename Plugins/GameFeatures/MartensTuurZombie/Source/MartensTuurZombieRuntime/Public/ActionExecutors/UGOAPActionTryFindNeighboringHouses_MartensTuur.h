#pragma once
#include "GOAP.h"
#include "StudentPerceptor.h"
#include "UGOAPActionTryFindNeighboringHouses_MartensTuur.generated.h"

UCLASS()
class UGOAPActionTryFindNeighboringHouses_MartensTuur : public UGOAPActionExecutor {
	GENERATED_BODY()
	
	UPROPERTY()
	FVector CurrentDestination{};
	
	UPROPERTY()
	UStudentPerceptor *CachedStudentPerceptor{};
	
	UPROPERTY()
	int NumHousesAtStart{};
	
public:
	virtual void Begin_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual EGOAPExecutorResult ExecutorTick_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
};
