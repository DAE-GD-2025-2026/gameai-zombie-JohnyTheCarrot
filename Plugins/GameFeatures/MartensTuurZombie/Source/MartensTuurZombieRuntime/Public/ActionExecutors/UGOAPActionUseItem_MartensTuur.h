#pragma once
#include "GOAP.h"
#include "Items/BaseItem.h"
#include "UGOAPActionUseItem_MartensTuur.generated.h"

UCLASS()
class UGOAPActionUseItem_MartensTuur : public UGOAPActionExecutor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY()
	TWeakObjectPtr<UInventoryComponent> CachedInventory;
	
	virtual ABaseItem* GetItem(int &SlotIdx) {return nullptr;}
	
public:
	virtual void Begin_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
	
	virtual EGOAPExecutorResult ExecutorTick_Implementation(UObject* WorldContextObject, AAIController* Controller) override;
};

UCLASS()
class UGOAPActionUseMedkit_MartensTuur : public UGOAPActionUseItem_MartensTuur
{
	GENERATED_BODY()
	
protected:
	virtual ABaseItem* GetItem(int &SlotIdx) override;
};

UCLASS()
class UGOAPActionUseFood_MartensTuur : public UGOAPActionUseItem_MartensTuur
{
	GENERATED_BODY()
	
protected:
	virtual ABaseItem* GetItem(int &SlotIdx) override;
};
