#pragma once
#include "FKnownHouse_MartensTuur.h"
#include "GOAP.h"
#include "StudentPerceptor.h"
#include "UGOAPActionScanHouseFor_MartensTuur.generated.h"

UCLASS()
class UGOAPActionScanHouseFor_MartensTuur : public UGOAPActionExecutor
{
	GENERATED_BODY()
	
protected:
	FKnownHouse_MartensTuur *House{};
	
	UPROPERTY()
	UStudentPerceptor *CachedStudentPerceptor{};
	
	[[nodiscard]]
	virtual bool IsDone() {return false;};
	
public:
	virtual void Begin_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual EGOAPExecutorResult ExecutorTick_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual void OnFinish() override;
};

UCLASS()
class UGoapActionScanHouseForWeapon_MartensTuur : public UGOAPActionScanHouseFor_MartensTuur
{
	GENERATED_BODY()
	
	UPROPERTY()
	int NumKnownWeaponsAtStart{};

protected:
	virtual void Begin_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual bool IsDone() override;
};

UCLASS()
class UGoapActionScanHouseForFood_MartensTuur : public UGOAPActionScanHouseFor_MartensTuur
{
	GENERATED_BODY()
	
	UPROPERTY()
	int NumKnownFoodsAtStart{};

protected:
	virtual void Begin_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual bool IsDone() override;
};

UCLASS()
class UGoapActionScanHouseForMedkit_MartensTuur : public UGOAPActionScanHouseFor_MartensTuur
{
	GENERATED_BODY()
	
	UPROPERTY()
	int NumKnownMedkitsAtStart{};

protected:
	virtual void Begin_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual bool IsDone() override;
};
