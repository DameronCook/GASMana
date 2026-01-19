// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AN_CamShake.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UAN_CamShake : public UAnimNotifyState
{
	GENERATED_BODY()

	[[deprecated]] virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	[[deprecated]] virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CamShake")
	TSubclassOf<UCameraShakeBase> CameraShake;

	UPROPERTY()
	UCameraShakeBase* CameraShakeInstance;
	
};
