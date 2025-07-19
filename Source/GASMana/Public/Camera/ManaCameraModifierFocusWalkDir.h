// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/ManaCameraModifier.h"
#include "ManaCameraModifierFocusWalkDir.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UManaCameraModifierFocusWalkDir : public UManaCameraModifier
{
	GENERATED_BODY()
	
public:
	UManaCameraModifierFocusWalkDir();

	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;
	virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

private:
	/*Rate at which the camera should follow the turning player, in degrees per second*/
	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float RotationSpeed;
	
	/*Minimum time between two changes in direction of auto cam rotation, in seconds*/
	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float DirectionChangeCooldown;
	
	/* Whether to always rotate the camera to focus walk direction, or only while the player is providing movement input*/
	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	bool RotateOnlyWhileCharacterIsMoving;

	float PreviousYawDeltaSign;
	float DirectionChangeCooldownRemaining;
};
