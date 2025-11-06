// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_StopAllMontages.h"
#include "AIController.h"
#include "GameFramework/Character.h"

UBTTask_StopAllMontages::UBTTask_StopAllMontages()
{
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_StopAllMontages::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	ACharacter* Char = Cast<ACharacter>(AIController->GetPawn());
	if (!Char) return EBTNodeResult::Failed;

	UAnimInstance* AnimInstance = Char->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return EBTNodeResult::Failed;
	
	AnimInstance->StopAllMontages(true);
	
	return EBTNodeResult::Succeeded;
}
