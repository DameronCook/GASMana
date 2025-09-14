// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_EquipWeapon.h"

#include "Actors/BaseManaEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTask_EquipWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ABaseManaEnemy* Char = Cast<ABaseManaEnemy>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor")))
	{
		if (AEquipment* Equipment = Char->EnemyEquip())
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject("RightHandEquipment", Equipment);
			return EBTNodeResult::Succeeded;
		}
	}
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
