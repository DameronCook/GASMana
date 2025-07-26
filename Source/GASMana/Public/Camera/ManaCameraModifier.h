// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "ManaCameraInfo.h"
#include "ManaCameraModifier.generated.h"

class AManaCameraModificationVolume;

/**
 * 
 */
UCLASS()
class GASMANA_API UManaCameraModifier : public UCameraModifier
{
	GENERATED_BODY()
	
public:
	virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;

	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;


	/* Gets the current camera modifiers that are being applied */
	FManaCameraInfo GetCurrentModifiers();

	/* Get the current camera modifiers that we are transitioning into */
	FManaCameraInfo GetTargetModifiers();

	/* Time to wait to apply changes automatically after the player has stopped inputing it */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	float CooldownAfterPlayerInput;

protected:
	/* Applies the specific camera properties with a transition time if necessary */
	void ApplyCameraTransition(const FManaCameraInfo& CamInfo, const float TransitionTime, struct FMinimalViewInfo& MinimalViewInfo, const float DeltaTime);

	/* Call this to check if the player has recently inputted to change the camera */
	bool PlayerHasRecentlyChangedCamera() const;

	bool WallRunning;
	bool IsBlocking; 
	bool IsZipToPoint;
	bool IsSwing;
	bool IsRoll;

private: 
	void ApplyCameraInfo(const FManaCameraInfo& CamInfo, const float Factor, struct FMinimalViewInfo& MinimalViewInfo) const;

	FManaCameraInfo CurrentModifiers;
	FManaCameraInfo TargetModifiers;
	float TotalTransitionTime;
	float RemainingTransitionTime;

	/* Time in seconds before we start automatically transitioning */
	float CooldownRemaining;
};
