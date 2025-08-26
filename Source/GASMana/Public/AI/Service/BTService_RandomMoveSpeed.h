// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_RandomMoveSpeed.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UBTService_RandomMoveSpeed : public UBTService
{
	GENERATED_BODY()

	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
