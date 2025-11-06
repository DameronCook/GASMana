// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayMontageNonBlocking.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UBTTask_PlayMontageNonBlocking : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UBTTask_PlayMontageNonBlocking();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	
	UPROPERTY(EditAnywhere, Category = "Default")
	UAnimMontage* MontageToPlay;
};
