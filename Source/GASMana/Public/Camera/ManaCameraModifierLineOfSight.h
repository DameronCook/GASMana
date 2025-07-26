// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/ManaCameraModifier.h"
#include "ManaCameraModifierLineOfSight.generated.h"

/**
 * Makes the camera try to always keep line of sight to its target.
 * Preemptively swings away from obstacles before they occlude the character.
 */
UCLASS()
class GASMANA_API UManaCameraModifierLineOfSight : public UManaCameraModifier
{
	GENERATED_BODY()
	
public:
	UManaCameraModifierLineOfSight();

	virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;
	
	virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;

	/*How far away from the camera should we look from the camera in radians*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float MaxAngleInRadians;

	/* How precisely should we look for obstacles*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float StepSizeRadians;

	/* Rate at which the camera should follow the player, radians per second */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float RotationSpeed;

	/* Collision Channel the camera should look for for verifying line of sight between taget (prob just visibility or camera) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	TEnumAsByte<ECollisionChannel> LineOfSightProbeChannel;

private:

	/* Checks to see whether To can be seen from From, with respect to probe size and chennel*/
	bool IsInLineOfSight(const FVector& From, const FVector& To) const;

	/*Rotates InOutViewLocation and Rotation around TargetLocation by AngleInRadians*/
	void RotateAroundLocation(const FVector& TargetLocation, FVector& InOutViewLocation, FRotator& InOutViewRotation, const float AngleInRadians);
};
