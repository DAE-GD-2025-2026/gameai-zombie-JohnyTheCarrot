#pragma once

#include "SurvivorAgentBehavior_MartensTuur.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTasks_MartensTuur.generated.h"

UCLASS(BlueprintType)
class UBTMarkHouseExplored : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTMarkHouseExplored();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector HouseKey;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

UCLASS(BlueprintType)
class UBTFindNewWanderPos : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTFindNewWanderPos();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MinAngle{-45.f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float MaxAngle{+45.f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Distance{200.f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	float Radius{150.f};
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector WanderKey;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

UCLASS(BlueprintType)
class UBTGetItem : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTGetItem();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FBlackboardKeySelector ItemKey;
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};

UCLASS(BlueprintType)
class UBTCombat_MartensTuur : public UBTTaskNode
{
	GENERATED_BODY()
	
	UPROPERTY()
	ASurvivorPawn *Pawn;
	
	UPROPERTY()
	TWeakObjectPtr<USurvivorAgentBehavior_MartensTuur> AgentBehavior;
	
	UPROPERTY()
	TObjectPtr<USteeringBehavior_Flee_MartensTuur> Flee;
	
	UPROPERTY()
	TObjectPtr<USteeringBehavior_Seek_MartensTuur> Seek;
	
	UPROPERTY()
	TObjectPtr<USteeringBehavior_Face_MartensTuur> Face;
	
	UPROPERTY()
	float SecondsSinceShot{};
	
	UPROPERTY()
	TObjectPtr<USteeringBehavior_Blended_MartensTuur> CombatBehavior;

public:
	UBTCombat_MartensTuur();
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
