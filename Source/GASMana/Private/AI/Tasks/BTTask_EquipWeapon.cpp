// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_EquipWeapon.h"

#include "Actors/BaseManaEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Item/LeftHandEquipment.h"
#include "Item/RightHandEquipment.h"

EBTNodeResult::Type UBTTask_EquipWeapon::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ABaseManaEnemy* Char = Cast<ABaseManaEnemy>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor")))
	{
		if (AEquipment* Equipment = Char->EnemyEquip())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Enemy Equip returned something");

			if (ARightHandEquipment* RightEquipment = Cast<ARightHandEquipment>(Equipment))
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsObject("RightHandEquipment", RightEquipment);
				return EBTNodeResult::Succeeded;
			}
			
			if (ALeftHandEquipment* LeftEquipment = Cast<ALeftHandEquipment>(Equipment))
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Setting Left hand equipment..");
				OwnerComp.GetBlackboardComponent()->SetValueAsObject("LeftHandEquipment", LeftEquipment);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
