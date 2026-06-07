#pragma once
#include "SteeringBehaviors_MartensTuur/SteeringBehavior_MartensTuur.h"
#include "Survivor/SurvivorPawn.h"
#include "SurvivorAgentBehavior_MartensTuur.generated.h"

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class USurvivorAgentBehavior_MartensTuur : public UActorComponent
{
	GENERATED_BODY()
	
	UPROPERTY()
	UFloatingPawnMovement *FloatingPawnMovement;
	
	TOptional<UE::Math::TRotator<double>> TargetRotator;
	
public:
	USurvivorAgentBehavior_MartensTuur();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSteeringBehaviorTarget_MartensTuur SteerTarget{};
	
	UFUNCTION(BlueprintCallable)
	USteeringBehavior_MartensTuur *GetSteeringBehavior() const;
	
	// A singular behavior being active is a blended behavior with one element
	// This set-up allows for run-time adding of more behaviors
	UPROPERTY()
	USteeringBehavior_MartensTuur *CurrentSteeringBehavior;
	
	UFUNCTION(BlueprintCallable)
	void SetCurrentSteeringBehavior(USteeringBehavior_MartensTuur *SteeringBehavior);
};
