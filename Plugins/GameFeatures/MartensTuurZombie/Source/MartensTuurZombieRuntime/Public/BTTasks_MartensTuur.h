#pragma once

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
