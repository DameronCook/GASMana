// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AN_ComboNotifyState.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UAN_ComboNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Combo, meta = (AllowPrivateAccess = "true"))
	FName NextComboAttack;
};
