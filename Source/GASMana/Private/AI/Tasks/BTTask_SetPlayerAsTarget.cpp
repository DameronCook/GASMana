// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_SetPlayerAsTarget.h"

#include "AI/AIC_NPC.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_SetPlayerAsTarget::UBTTask_SetPlayerAsTarget()
{
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_SetPlayerAsTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIC_NPC* AIController = Cast<AAIC_NPC>(OwnerComp.GetAIOwner());
	if (!AIController) return EBTNodeResult::Failed;

	AActor* Actor = AIController->GetSensedActor();
	if (!Actor) return EBTNodeResult::Failed;

	OwnerComp.GetBlackboardComponent()->SetValueAsObject("TargetToFollow", Actor);

	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
