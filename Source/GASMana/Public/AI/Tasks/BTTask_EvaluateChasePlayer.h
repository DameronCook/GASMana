// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_EvaluateChasePlayer.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UBTTask_EvaluateChasePlayer : public UBTTaskNode
{
	GENERATED_BODY()

	UBTTask_EvaluateChasePlayer();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
