#include "UGOAPActionUseItem_MartensTuur.h"

#include "StudentPerceptor_MartensTuur.h"
#include "Common/InventoryComponent.h"

void UGOAPActionUseItem_MartensTuur::Begin_Implementation(UObject* WorldContextObject, AAIController* Controller)
{
	Super::Begin_Implementation(WorldContextObject, Controller);
	
	CachedInventory = GetOwner()->GetComponentByClass<UInventoryComponent>();
	check(CachedInventory != nullptr);
	
	int SlotIdx = -1;
	auto const Item = GetItem(SlotIdx);
	
	if (Item == nullptr)
	{
		Finish(EGOAPExecutorResult::Failure);
		return;
	}
	
	Item->UseItem(*Cast<ASurvivorPawn>(Controller->GetPawn()));
	if (Item->GetValue() == 0)
	{
		CachedInventory->RemoveItem(SlotIdx);
	}
	auto const Perceptor = GetOwner()->GetComponentByClass<UStudentPerceptor_MartensTuur>();
	Perceptor->RefreshSurvivorState();
	Finish(EGOAPExecutorResult::Success);
}

EGOAPExecutorResult UGOAPActionUseItem_MartensTuur::ExecutorTick_Implementation(UObject* WorldContextObject,
	AAIController* Controller)
{
	return Status;
}

ABaseItem* UGOAPActionUseMedkit_MartensTuur::GetItem(int &SlotIdx)
{
	auto const &Inv = CachedInventory->GetInventory();
	
	for (int I = 0; I < Inv.Num(); ++I)
	{
		auto const Item = Inv[I];
		
		SlotIdx = I;
		if (Item == nullptr) continue;
		if (Item->GetItemType() == EItemType::Medkit)
		{
			return Item;
		}
	}

	return nullptr;
}

ABaseItem* UGOAPActionUseFood_MartensTuur::GetItem(int &SlotIdx)
{
	auto const &Inv = CachedInventory->GetInventory();
	
	for (int I = 0; I < Inv.Num(); ++I)
	{
		auto const Item = Inv[I];
		
		SlotIdx = I;
		if (Item == nullptr) continue;
		if (Item->GetItemType() == EItemType::Food)
		{
			return Item;
		}
	}

	return nullptr;
}
