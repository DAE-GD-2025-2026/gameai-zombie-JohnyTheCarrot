#pragma once
#include "GOAP.h"

#include "GOAPPlanning.generated.h"

using GoapGraphNode = TObjectPtr<UGOAPActionAsset>;

UCLASS(Blueprintable, ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class MARTENSTUURZOMBIERUNTIME_API UGoapGraph : public UActorComponent
{
public:
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GOAP")
	TArray<TObjectPtr<UGOAPActionAsset>> AvailableActions;
	
	using GoapPlan = TArray<UGOAPActionAsset*>;
	
	UFUNCTION(BlueprintCallable, Category="GOAP")
	void InitializeGoap();
	
	UFUNCTION(BlueprintCallable, Category="GOAP")
	TArray<UGOAPActionAsset*> Plan(EGOAPState StartState, UGoal *Goal) const;
};

UCLASS(Category="GOAP")
class MARTENSTUURZOMBIERUNTIME_API  UGOAPPlanner_MartensTuur : public UGoapGraph
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="GOAP")
	EGOAPState State{};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GOAP")
	TArray<UGOAPActionAsset*> CurrentPlan;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="GOAP")
	TObjectPtr<UGoal> CurrentGoal{};
	
	UPROPERTY()
	int CurrentActionIndex{0};
	
	[[nodiscard]]
	UGOAPActionAsset *GetCurrentAction() const;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
