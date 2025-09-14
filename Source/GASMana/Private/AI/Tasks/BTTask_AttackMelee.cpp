// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_AttackMelee.h"

#include "Actors/BaseManaEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_AttackMelee::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	OwnerActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));
	
	if (AGASManaCharacter* Char = Cast<AGASManaCharacter>(OwnerActor))
	{
		if (Char->Attack())
		{
			return EBTNodeResult::Succeeded;	
		}
	}
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
