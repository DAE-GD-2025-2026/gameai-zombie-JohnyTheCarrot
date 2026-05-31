#pragma once
#include "GOAP.h"

#include "GOAPPlanning.generated.h"

using GoapGraphNode = TObjectPtr<UGOAPActionAsset>;

UCLASS(Blueprintable, ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class MARTENSTUURZOMBIERUNTIME_API UGoapGraph : public UActorComponent
{
	void NextAction();
	
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
	TObjectPtr<UGoal> CurrentGoal{};
	
	UPROPERTY()
	int CurrentActionIndex{-1};
	
	[[nodiscard]]
	UGOAPActionAsset *GetCurrentAction() const;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
