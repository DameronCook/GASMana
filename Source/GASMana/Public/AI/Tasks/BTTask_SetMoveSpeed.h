// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BTTask_SetMoveSpeed.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UBTTask_SetMoveSpeed : public UBTTaskNode
{
	GENERATED_BODY()

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
	UPROPERTY(EditAnywhere, Category = "Default")
	float NewWalkSpeed;
};
