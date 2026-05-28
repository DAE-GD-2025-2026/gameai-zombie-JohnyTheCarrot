#pragma once
#include "GOAP.h"

#include "GOAPPlanning.generated.h"

using GoapGraphNode = TObjectPtr<UGOAPActionAsset>;

UCLASS(Blueprintable, ClassGroup=(AI), meta=(BlueprintSpawnableComponent))
class MARTENSTUURZOMBIERUNTIME_API UGoapGraph : public UActorComponent
{
public:
	GENERATED_BODY()
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="GOAP")
	TArray<TObjectPtr<UGOAPActionAsset>> AvailableActions;
	
	using GoapPlan = TArray<UGOAPActionAsset*>;
	
	UFUNCTION(BlueprintCallable, Category="GOAP")
	void InitializeGoap();
	
	UFUNCTION(BlueprintCallable, Category="GOAP")
	TArray<UGOAPActionAsset*> Plan(EGOAPState StartState, UGoal *Goal) const;
};