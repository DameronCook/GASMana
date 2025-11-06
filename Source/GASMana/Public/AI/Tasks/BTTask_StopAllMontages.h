// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_StopAllMontages.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UBTTask_StopAllMontages : public UBTTaskNode
{
	GENERATED_BODY()
public:

	UBTTask_StopAllMontages();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
