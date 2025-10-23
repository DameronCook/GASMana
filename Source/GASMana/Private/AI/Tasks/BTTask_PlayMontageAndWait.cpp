// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_PlayMontageAndWait.h"

#include "AIController.h"
#include "GameFramework/Character.h"

UBTTask_PlayMontageAndWait::UBTTask_PlayMontageAndWait() : BehaviorTreeComp()
{
	bNotifyTaskFinished = true;

	bCreateNodeInstance = true;

	BehaviorTreeComp = nullptr;
	AIController = nullptr;
	AnimInstance = nullptr;
}

void UBTTask_PlayMontageAndWait::OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted)
{
	if (AnimMontage != MontageToPlay) return;

	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UBTTask_PlayMontageAndWait::OnMontageEnded);
	}
	
	if (BehaviorTreeComp)
	{
		FinishLatentTask(*BehaviorTreeComp, EBTNodeResult::Succeeded);
	}

	BehaviorTreeComp = nullptr;
	AIController = nullptr;
	AnimInstance = nullptr;
}


EBTNodeResult::Type UBTTask_PlayMontageAndWait::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	BehaviorTreeComp = &OwnerComp;
	
	AIController = OwnerComp.GetAIOwner();
	if (!AIController) return EBTNodeResult::Failed;

	const ACharacter* Char = Cast<ACharacter>(AIController->GetPawn());
	if (!Char) return EBTNodeResult::Failed;

	AnimInstance = Char->GetMesh()->GetAnimInstance();
	if (!AnimInstance) return EBTNodeResult::Failed;

	if (!MontageToPlay)	return EBTNodeResult::Failed;
	
	AnimInstance->Montage_Play(MontageToPlay);

	AnimInstance->OnMontageEnded.AddDynamic(this, &UBTTask_PlayMontageAndWait::OnMontageEnded);
	
	return EBTNodeResult::InProgress;
}

void UBTTask_PlayMontageAndWait::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	// If the task was aborted/finished in another way, make sure we unbind the delegate
	if (AnimInstance)
	{
		AnimInstance->OnMontageEnded.RemoveDynamic(this, &UBTTask_PlayMontageAndWait::OnMontageEnded);
		AnimInstance = nullptr;
	}

	AIController = nullptr;
	BehaviorTreeComp = nullptr;

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}