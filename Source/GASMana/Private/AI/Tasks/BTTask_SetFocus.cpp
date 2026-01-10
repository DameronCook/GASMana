// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_SetFocus.h"

#include "AI/AIC_NPC.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetFocus::UBTTask_SetFocus()
{
}

EBTNodeResult::Type UBTTask_SetFocus::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AAIC_NPC* AIController = Cast<AAIC_NPC>(OwnerComp.GetAIOwner()))
	{
		if (const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent())
		{
			if (AActor* TargetActor = Cast<AActor>(BlackboardComp->GetValueAsObject(FocusTarget.SelectedKeyName)))
			{
				AIController->SetFocus(TargetActor);
				return EBTNodeResult::Succeeded;
			}
		}
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
	return EBTNodeResult::Failed;
}
