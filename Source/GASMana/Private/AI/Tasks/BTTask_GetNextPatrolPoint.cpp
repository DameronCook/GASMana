// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_GetNextPatrolPoint.h"

#include "Actors/BaseManaEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_GetNextPatrolPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ABaseManaEnemy* Char = Cast<ABaseManaEnemy>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor")))
	{
		const FVector NextPoint = Char->GetNextPatrolPoint();
		OwnerComp.GetBlackboardComponent()->SetValueAsVector("MoveToLocation", NextPoint);
		return EBTNodeResult::Succeeded;
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
