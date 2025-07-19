// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaCameraModificationVolume.h"
#include "Interface/CameraActorInterface.h"

AManaCameraModificationVolume::AManaCameraModificationVolume()
{
	TransitionTime = 1.0f;
}

void AManaCameraModificationVolume::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);

	ICameraActorInterface* CameraActor = Cast<ICameraActorInterface>(Other);

	if (!CameraActor) return;

	CameraActor->SetCurrentCameraModificationVolume(this);
}

void AManaCameraModificationVolume::NotifyActorEndOverlap(AActor* Other)
{
	Super::NotifyActorEndOverlap(Other);

	ICameraActorInterface* CameraActor = Cast<ICameraActorInterface>(Other);

	if (!CameraActor) return;

	if (CameraActor->GetCurrentCameraModificationVolume() == this)
	{
		CameraActor->SetCurrentCameraModificationVolume(nullptr);
	}
}

