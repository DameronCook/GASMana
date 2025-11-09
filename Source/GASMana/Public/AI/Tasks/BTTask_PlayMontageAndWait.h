// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_PlayMontageAndWait.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UBTTask_PlayMontageAndWait : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_PlayMontageAndWait();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
	virtual EBTNodeResult::Type AbortTask ( UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
protected:
	UFUNCTION()
	void OnMontageEnded(UAnimMontage* AnimMontage, bool bInterrupted);

	UPROPERTY(EditAnywhere, Category = "Default")
	UAnimMontage* MontageToPlay;
private:
	UPROPERTY()
	AAIController* AIController;
	UPROPERTY()
	ACharacter* Character;
	UPROPERTY()
	UBehaviorTreeComponent* BehaviorTreeComp;
	UPROPERTY()
	UAnimInstance* AnimInstance;
};
