// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_WasMyItemPickedUp.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Item/Equipment.h"

UBTDecorator_WasMyItemPickedUp::UBTDecorator_WasMyItemPickedUp()
{
	FlowAbortMode = EBTFlowAbortMode::Self;
}

bool UBTDecorator_WasMyItemPickedUp::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (const AEquipment* Equipment = Cast<AEquipment>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetToFollow")))
	{
		return !Equipment->IsPickedUp();
	}
	return false;
}
