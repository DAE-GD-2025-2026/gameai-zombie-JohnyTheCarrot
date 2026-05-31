#include "UGOAPActionFightEnemy_MartensTuur.h"

#include "StudentPerceptor.h"
#include "Common/InventoryComponent.h"

void UGOAPActionFightEnemy_MartensTuur::Begin_Implementation(UObject* WorldContextObject, AAIController* Controller)
{
	Super::Begin_Implementation(WorldContextObject, Controller);
	
	StudentPerceptor = GetOwner()->GetComponentByClass<UStudentPerceptor>();
	check(StudentPerceptor != nullptr);
	
	if (StudentPerceptor->LastSeenZombiePos.IsEmpty())
	{
		UE_LOG(LogTemp, Warning, TEXT("No zombies to fight..?"));
		Finish(EGOAPExecutorResult::Failure);
		return;
	}
}

EGOAPExecutorResult UGOAPActionFightEnemy_MartensTuur::ExecutorTick_Implementation(UObject* WorldContextObject,
	AAIController* Controller)
{
	auto const InvComp = GetOwner()->GetComponentByClass<UInventoryComponent>();
	auto const Inv = InvComp->GetInventory();
	AWeapon *Weapon = nullptr;
	int SlotIdx = 0;
	for (int I = 0; I < Inv.Num(); ++I)
	{
		auto const Item = Inv[I];
		
		if (Item == nullptr) continue;
		if (Item->GetItemType() == EItemType::Pistol || Item->GetItemType() == EItemType::Shotgun)
		{
			Weapon = Cast<AWeapon>(Item);
			SlotIdx = I;
			break;
		}
	}
	if (!Weapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("No gun!"));
		return EGOAPExecutorResult::Failure;
	}

	// TODO: for non-research, make it so only zombies we can see are in this logic
	for (auto const &[ZombiePtr, Pos] : StudentPerceptor->LastSeenZombiePos)
	{
		if (!ZombiePtr.IsValid()) continue;
		// TODO: for non-research, no instant turning!
		auto const Dir = ZombiePtr->GetActorLocation() - GetOwner()->GetActorLocation();
		
		FRotator Rot = Dir.Rotation();
		
		Controller->GetPawn()->SetActorRotation(Rot);
		UE_LOG(LogTemp, Warning, TEXT("Shooting.."));
		InvComp->UseItem(SlotIdx);
		
		if (Weapon->GetValue() == 0)
		{
			InvComp->RemoveItem(SlotIdx);
			UE_LOG(LogTemp, Warning, TEXT("Out of ammo"));
			break;
		}
	}
	
	for (auto const &[ZombiePtr, Pos] : StudentPerceptor->LastSeenZombiePos)
	{
		if (ZombiePtr.IsValid())
			return EGOAPExecutorResult::Failure;
	}
	
	return EGOAPExecutorResult::Success;
}
