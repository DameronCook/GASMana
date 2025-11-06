// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_PlayMontageNonBlocking.h"

#include "AIController.h"
#include "GameFramework/Character.h"

UBTTask_PlayMontageNonBlocking::UBTTask_PlayMontageNonBlocking()
{
	bCreateNodeInstance = true;
	bNotifyTaskFinished = false;
}

EBTNodeResult::Type UBTTask_PlayMontageNonBlocking::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	const ACharacter* Char = Cast<ACharacter>(AIController->GetPawn());
	if (!Char) return EBTNodeResult::Failed;

	UAnimInstance* AnimInstance = Char->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return EBTNodeResult::Failed;

	if (!MontageToPlay)	return EBTNodeResult::Failed;

	AnimInstance->Montage_Play(MontageToPlay);
	
	return EBTNodeResult::Succeeded;
}
