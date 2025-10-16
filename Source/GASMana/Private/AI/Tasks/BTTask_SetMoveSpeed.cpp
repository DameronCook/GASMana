// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_SetMoveSpeed.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

EBTNodeResult::Type UBTTask_SetMoveSpeed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (const ACharacter* Char = Cast<ACharacter>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor")))
	{
		Char->GetCharacterMovement()->MaxWalkSpeed = NewWalkSpeed;
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
