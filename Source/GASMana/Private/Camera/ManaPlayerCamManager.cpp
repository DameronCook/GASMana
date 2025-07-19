// Fill out your copyright notice in the Description page of Project Settings.
#include "Camera/ManaPlayerCamManager.h"
#include "Camera/CameraModifier.h"

AManaPlayerCamManager::AManaPlayerCamManager()
{
    LineOfSightProbeSize = 12.f;
    LineOfSightProbeChannel = ECC_Camera;
}

void AManaPlayerCamManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
    Super::UpdateViewTarget(OutVT, DeltaTime);
}
