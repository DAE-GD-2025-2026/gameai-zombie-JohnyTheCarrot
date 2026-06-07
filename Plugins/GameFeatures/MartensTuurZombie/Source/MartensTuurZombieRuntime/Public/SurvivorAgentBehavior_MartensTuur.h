#pragma once
#include "Items/Food.h"
#include "Items/Medkit.h"
#include "Items/Weapon.h"
#include "SteeringBehaviors_MartensTuur/SteeringBehavior_MartensTuur.h"
#include "Survivor/SurvivorPawn.h"
#include "Village/House/House.h"
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
	
	TWeakObjectPtr<UHealthComponent> HealthComp{};
	
	TWeakObjectPtr<UStaminaComponent> StaminaComp{};
	
	TWeakObjectPtr<UInventoryComponent> InventoryComp{};
	
	[[nodiscard]]
	TOptional<int> GetFreeInvSlot() const;
	
	[[nodiscard]]
	bool ContainsItemType(EItemType Type) const;
	
	[[nodiscard]]
	AHouse *GetClosestHouse() const;
	
	[[nodiscard]]
	ABaseItem *GetClosestItemOfType(EItemType Type) const;
	
public:
	USurvivorAgentBehavior_MartensTuur();
	
	[[nodiscard]]
	bool GrabItem(ABaseItem *Item);
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSteeringBehaviorTarget_MartensTuur SteerTarget{};
	
	UFUNCTION(BlueprintCallable)
	USteeringBehavior_MartensTuur *GetSteeringBehavior() const;
	
	void InformAboutHouse(AHouse *House);
	void InformAboutItem(ABaseItem *Food);
	
	void MarkChecked(AHouse *House);
	
	UFUNCTION(BlueprintCallable)
	void UpdateBlackboard(UBlackboardComponent *Blackboard) const;
	
	// A singular behavior being active is a blended behavior with one element
	// This set-up allows for run-time adding of more behaviors
	UPROPERTY()
	USteeringBehavior_MartensTuur *CurrentSteeringBehavior;
	
	UFUNCTION(BlueprintCallable)
	void SetCurrentSteeringBehavior(USteeringBehavior_MartensTuur *SteeringBehavior);
};
