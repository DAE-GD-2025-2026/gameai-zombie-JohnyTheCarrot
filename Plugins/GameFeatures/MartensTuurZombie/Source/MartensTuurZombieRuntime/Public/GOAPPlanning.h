#pragma once
#include "GOAP.h"
#include "Common/HealthComponent.h"
#include "Common/StaminaComponent.h"
#include "GOAPPlanning.generated.h"

using GoapGraphNode = TObjectPtr<UGOAPActionAsset>;

UCLASS(Blueprintable, ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class MARTENSTUURZOMBIERUNTIME_API UGoapGraph : public UActorComponent
{
	void NextAction();
	
	UPROPERTY()
	UHealthComponent *HealthComp{};
	
	UPROPERTY()
	UStaminaComponent *StaminaComp{};
	
public:
	UGoapGraph();
	
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GOAP")
	TArray<TObjectPtr<UGOAPActionAsset>> AvailableActions;
	
	using GoapPlan = TArray<UGOAPActionAsset*>;
	
	UFUNCTION(BlueprintCallable, Category="GOAP")
	void InitializeGoap();
	
	UFUNCTION(BlueprintCallable, Category="GOAP")
	TArray<UGOAPActionAsset*> Plan(FGOAPState_Martens_Tuur StartState, UGoal *Goal) const;
	
	UFUNCTION(BlueprintCallable, Category="GOAP")
	void ActivatePlan(TArray<UGOAPActionAsset*> const &Plan);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GOAP")
	FGOAPState_Martens_Tuur State{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GOAP")
	TArray<UGOAPActionAsset*> CurrentPlan;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GOAP")
	TWeakObjectPtr<UGoal> CurrentGoal{};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GOAP")
	TArray<TObjectPtr<UGoal>> Goals{};
	
	UPROPERTY()
	int CurrentActionIndex{-1};
	
	void ActivateHighestPriorityGoal();
	
	[[nodiscard]]
	UGOAPActionAsset *GetCurrentAction() const;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
