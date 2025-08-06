// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/ManaCameraModifier.h"
#include "ManaCameraModifierInterpPitch.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UManaCameraModifierInterpPitch : public UManaCameraModifier
{
	GENERATED_BODY()
	
public:
	virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

	float ClampAngle(float CurrentAngle);

	/*Rate at which the camera should rotate back to zero, in degrees per second*/
	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float RotationSpeed;

	/*Minimum time between two changes in direction of auto cam rotation, in seconds*/
	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float DirectionChangeCooldown;

	float PreviousPitchDeltaSign;
	float DirectionChangeCooldownRemaining;
};
