// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaCameraModifierInterpPitch.h"

#include "Camera/PlayerCameraManager.h"
#include "Camera/ManaPlayerCamManager.h"

bool UManaCameraModifierInterpPitch::ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

	if (PlayerHasRecentlyChangedCamera())
	{
		return false;
	}

	float CurrentAngle = CameraOwner->GetCameraRotation().Pitch;
	
	CurrentAngle = ClampAngle(CurrentAngle);

	constexpr float DesiredAngle = -10.f;

	float PitchDelta = FMath::FindDeltaAngleDegrees(CurrentAngle, DesiredAngle);

	PitchDelta = ClampAngle(PitchDelta);
	OutViewRotation.Pitch = ClampAngle(OutViewRotation.Pitch);

	//GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Red, FString::Printf(TEXT("Pitch amount is: %f"), OutViewRotation.Pitch));



	float PitchDeltaSign = FMath::Sign(PitchDelta);

	float AppliedPitchDelta = PitchDelta * RotationSpeed * DeltaTime;

	if (FMath::Abs(PitchDelta) < FMath::Abs(AppliedPitchDelta))
	{
		PreviousPitchDeltaSign = 0.f;
		DirectionChangeCooldownRemaining = DirectionChangeCooldown;
		AppliedPitchDelta = PitchDelta;
	}


	OutDeltaRot.Pitch += AppliedPitchDelta;
	return false;
}

float UManaCameraModifierInterpPitch::ClampAngle(float CurrentAngle)
{
	while (CurrentAngle < -180.0f)
	{
		CurrentAngle += 360.0f;
	}

	while (CurrentAngle > 180.0f)
	{
		CurrentAngle -= 360.0f;
	}

	return CurrentAngle;
}
