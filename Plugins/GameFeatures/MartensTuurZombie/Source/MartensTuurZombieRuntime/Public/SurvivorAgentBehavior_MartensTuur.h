#pragma once
#include "Items/Food.h"
#include "Items/Medkit.h"
#include "Items/Weapon.h"
#include "SteeringBehaviors_MartensTuur/SteeringBehavior_MartensTuur.h"
#include "Survivor/SurvivorPawn.h"
#include "Village/House/House.h"
#include "Zombies/BaseZombie.h"
#include "SurvivorAgentBehavior_MartensTuur.generated.h"

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class USurvivorAgentBehavior_MartensTuur : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY()
	UFloatingPawnMovement *FloatingPawnMovement;
	
	TOptional<UE::Math::TRotator<double>> TargetRotator;
	
	TArray<TWeakObjectPtr<AHouse>> UncheckedHouses;
	TArray<TWeakObjectPtr<AHouse>> CheckedHouses;
	TArray<TWeakObjectPtr<ABaseItem>> KnownItems;
	TArray<TWeakObjectPtr<ABaseZombie>> KnownZombies;
	
	TWeakObjectPtr<UHealthComponent> HealthComp{};
	
	TWeakObjectPtr<UStaminaComponent> StaminaComp{};
	
	TWeakObjectPtr<UInventoryComponent> InventoryComp{};
	
	[[nodiscard]]
	TOptional<int> GetFreeInvSlot() const;
	
	[[nodiscard]]
	bool InvContainsItemType(EItemType Type) const;
	
	[[nodiscard]]
	bool AwareOfItemType(EItemType Type) const;
	
	[[nodiscard]]
	AHouse *GetClosestHouse() const;
	
	[[nodiscard]]
	ABaseItem *GetClosestItemOfType(EItemType Type) const;
	
	void UseItem(int Idx) const;
	
	[[nodiscard]]
	bool IsHungry() const;
	
	[[nodiscard]]
	bool IsHurt() const;
	
	[[nodiscard]]
	float ScoreItemInterest(ABaseItem *Item, float *ProximityScore) const;
	
	UPROPERTY()
	TWeakObjectPtr<ABaseItem> DesiredItem;
	
	UPROPERTY()
	int DesiredItemSlot{};
	
	UPROPERTY()
	int LastHealthAmount{};
	
	mutable bool bWasSurpriseAttacked{false};
	
public:
	USurvivorAgentBehavior_MartensTuur();
	
	[[nodiscard]]
	bool GrabItem(ABaseItem *Item);
	
	[[nodiscard]]
	bool GrabDesiredItem();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void TickSteeringBehavior(USteeringBehavior_MartensTuur *Behavior, FSteeringBehaviorTarget_MartensTuur const &Target, float DeltaTime, float
	                          Scale);
	
	void Shoot();
	
	[[nodiscard]]
	float GetSafeEnemyDistance() const;
	
	[[nodiscard]]
	TArray<TWeakObjectPtr<ABaseZombie>> const &GetKnownZombies() const
	{
		return KnownZombies;
	}
	
	void InformAboutHouse(AHouse *House);
	void InformAboutItem(ABaseItem *Food);
	
	void MarkChecked(AHouse *House);
	
	void HasSeenZombie(ABaseZombie *Zombie);
	
	UFUNCTION(BlueprintCallable)
	void UpdateBlackboard(UBlackboardComponent *Blackboard) const;
};
