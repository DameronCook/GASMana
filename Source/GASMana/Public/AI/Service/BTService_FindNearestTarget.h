// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindNearestTarget.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UBTService_FindNearestTarget : public UBTService
{
	GENERATED_BODY()

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetToFollow;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DistToTarget;
};
