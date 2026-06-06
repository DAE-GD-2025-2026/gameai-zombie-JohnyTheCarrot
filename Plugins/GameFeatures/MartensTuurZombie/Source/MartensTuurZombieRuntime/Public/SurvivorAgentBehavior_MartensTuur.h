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
	
public:
	USurvivorAgentBehavior_MartensTuur();
	
	virtual void BeginPlay() override;
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSteeringBehaviorTarget_MartensTuur SteerTarget{};
	
	UPROPERTY(BlueprintReadOnly)
	USteeringBehavior_MartensTuur *CurrentSteeringBehavior;
	
	UFUNCTION(BlueprintCallable)
	void SetCurrentSteeringBehavior(USteeringBehavior_MartensTuur *SteeringBehavior);
	
	UFUNCTION(BlueprintCallable)
	void MoveInDirection(float DeltaTime, FVector2D Direction, float Scale = 1.f);
};
