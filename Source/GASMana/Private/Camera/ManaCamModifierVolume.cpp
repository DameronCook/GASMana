// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaCamModifierVolume.h"

#include "Camera/CameraTypes.h"

#include "Interface/CameraActorInterface.h"
#include "Camera/ManaCameraModificationVolume.h"

UManaCamModifierVolume::UManaCamModifierVolume()
{
}

bool UManaCamModifierVolume::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	Super::ModifyCamera(DeltaTime, InOutPOV);
	
	ICameraActorInterface* CamActor = Cast<ICameraActorInterface>(GetViewTarget());

	if (!CamActor)
	{
		return false;
	}

	const FManaCameraInfo PlayerCameraModificationVolumeInfo = CamActor->GetCurrentCameraModificationVolume() ? CamActor->GetCurrentCameraModificationVolume()->Modifiers : FManaCameraInfo();

	const float TransitionTime = CamActor->GetCurrentCameraModificationVolume() ? CamActor->GetCurrentCameraModificationVolume()->TransitionTime : 0.0f;

	ApplyCameraTransition(PlayerCameraModificationVolumeInfo, TransitionTime, InOutPOV, DeltaTime);
	
	return false;
}
