// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "ManaPlayerCamManager.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API AManaPlayerCamManager : public APlayerCameraManager
{
	GENERATED_BODY()
	
	AManaPlayerCamManager();

public:

	/* Radius of Probe for verifying line of sight between camera and view target*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera)
	float LineOfSightProbeSize;

	/* Collision Channel of the query probe for verifying line of sight between camera and view target*/
	UPROPERTY()
	TEnumAsByte<ECollisionChannel> LineOfSightProbeChannel;

protected:

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
};
