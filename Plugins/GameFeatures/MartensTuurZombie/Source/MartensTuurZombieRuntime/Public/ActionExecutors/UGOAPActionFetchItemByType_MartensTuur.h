#pragma once
#include "GOAP.h"
#include "StudentPerceptor.h"
#include "Common/InventoryComponent.h"
#include "Items/Weapon.h"
#include "UGOAPActionFetchItemByType_MartensTuur.generated.h"

UCLASS()
class UGOAPActionFetchItemByType_MartensTuur : public UGOAPActionExecutor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	TWeakObjectPtr<ABaseItem> Target{};
	
	UPROPERTY()
	UStudentPerceptor *CachedPerceptor{};
	
	UPROPERTY()
	UInventoryComponent* CachedInventory{};
	
	virtual void SelectTarget() {}
	
	virtual void RemoveFromKnown() {};
	
public:
	virtual void Begin_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual EGOAPExecutorResult ExecutorTick_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
};

UCLASS()
class UGOAPActionFetchWeapon_MartensTuur : public UGOAPActionFetchItemByType_MartensTuur
{
	GENERATED_BODY()
	
protected:
	virtual void SelectTarget() override;
	virtual void RemoveFromKnown() override;
};

UCLASS()
class UGOAPActionFetchFood_MartensTuur : public UGOAPActionFetchItemByType_MartensTuur
{
	GENERATED_BODY()
	
protected:
	virtual void SelectTarget() override;
	virtual void RemoveFromKnown() override;
};

UCLASS()
class UGOAPActionFetchMedkit_MartensTuur : public UGOAPActionFetchItemByType_MartensTuur
{
	GENERATED_BODY()
	
protected:
	virtual void SelectTarget() override;
	virtual void RemoveFromKnown() override;
};
