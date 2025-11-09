// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_EvaluateChasePlayer.h"

#include "AI/AIC_NPC.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_EvaluateChasePlayer::UBTTask_EvaluateChasePlayer()
{
}

EBTNodeResult::Type UBTTask_EvaluateChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIC_NPC* AIController = Cast<AAIC_NPC>(OwnerComp.GetAIOwner());
	if (!AIController) return EBTNodeResult::Failed;

	AActor* Actor = AIController->GetSensedActor();
	if (!Actor) return EBTNodeResult::Failed;

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return EBTNodeResult::Failed;

	if (!Blackboard->GetValueAsBool("CanSeePlayer"))
	{
		Blackboard->SetValueAsObject("TargetToFollow", nullptr);
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}
