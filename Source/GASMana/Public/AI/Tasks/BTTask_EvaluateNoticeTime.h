// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EvaluateNoticeTime.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UBTTask_EvaluateNoticeTime : public UBTTaskNode
{
	GENERATED_BODY()

	UBTTask_EvaluateNoticeTime();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

public:
	UPROPERTY(EditAnywhere, Category = "Time")
	float MinTimeToWait = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Time")
	float MinDist = 1500.f;

	UPROPERTY(EditAnywhere, Category = "Time")
	float MaxTimeToWait = 12.f;

	UPROPERTY(EditAnywhere, Category = "Time")
	float MaxDistance = 5000.f;
	
};
