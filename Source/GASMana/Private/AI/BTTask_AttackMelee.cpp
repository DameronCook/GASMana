// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_AttackMelee.h"

#include "Actors/BaseManaEnemy.h"

EBTNodeResult::Type UBTTask_AttackMelee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	EBTNodeResult::Type FinishTask = EBTNodeResult::InProgress;
	
	if (const ABaseManaEnemy* Enemy = Cast<ABaseManaEnemy>(OwnerComp.GetOwner()))
	{
		if (Enemy->DoMeleeAttack())
		{
			FinishTask = EBTNodeResult::Succeeded;	
		}
	}
	return FinishTask;
}