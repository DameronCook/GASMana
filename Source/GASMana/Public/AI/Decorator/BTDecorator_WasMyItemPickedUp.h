// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_WasMyItemPickedUp.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UBTDecorator_WasMyItemPickedUp : public UBTDecorator
{
	GENERATED_BODY()
	UBTDecorator_WasMyItemPickedUp();
	
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
};
