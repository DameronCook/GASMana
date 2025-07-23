// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/ManaCameraModifier.h"
#include "ManaCameraModifierFocusNewActor.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UManaCameraModifierFocusNewActor : public UManaCameraModifier
{
	GENERATED_BODY()
	
	UManaCameraModifierFocusNewActor();

public:
	virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

private:
	/* Rate at which the camera moves while not snapping to the target, in degress per second*/
	UPROPERTY(EditDefaultsOnly, Category = Custom, meta = (AllowPrivateAccess = "true"))
	float RotationSpeed;

	/* Rate at which the camera should snap to target, in degress per second*/
	UPROPERTY(EditDefaultsOnly, Category = Custom, meta = (AllowPrivateAccess = "true"))
	float SnapSpeed;

	/** Target actor that was selected at the time of the previous camera update. */
	AActor* LastTarget;

	/** Desired rotation for focusing the target actor. */
	FRotator DesiredRotation;
};
