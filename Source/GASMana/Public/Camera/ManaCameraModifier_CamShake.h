// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/ManaCameraModifier.h"
#include "ManaCameraModifier_CamShake.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UManaCameraModifier_CamShake : public UManaCameraModifier
{
	GENERATED_BODY()

	UManaCameraModifier_CamShake();

	float ShakeTime;

	virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;
public:
	void UpdateShake(float nShakeTime);
	FRotator GetDeltaCameraRotation(float DeltaTime, float InterpSpeed, const FRotator& CurrentRotation,
	                                const FRotator& FinalRotation);
};
