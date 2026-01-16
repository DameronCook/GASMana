// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaCameraModifier_CamShake.h"
#include "PlayerManaCharacter.h"
#include "Camera/ManaSpringArmComponent.h"

class UManaSpringArmComponent;

UManaCameraModifier_CamShake::UManaCameraModifier_CamShake()
{
}

bool UManaCameraModifier_CamShake::ProcessViewRotation(class AActor* ViewTarget, float DeltaTime,
	FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	if (ShakeTime >= 0)
	{
		if (const APlayerManaCharacter* PlayerChar = Cast<APlayerManaCharacter>(ViewTarget))
		{
			FRotator NewCameraRotation = FRotator(OutViewRotation.Pitch, OutViewRotation.Yaw, 0.f);
			FVector TargetCamSocketOffset = FVector(0.f, 40.f, 40.f);
			float InterpRotationSpeed = 3.f;
			
			UManaSpringArmComponent* PlayerSpringArm = PlayerChar->GetCameraBoom();
			PlayerSpringArm->SocketOffset = FMath::VInterpTo(PlayerSpringArm->SocketOffset, TargetCamSocketOffset, DeltaTime, InterpRotationSpeed);

			const FRotator CameraDelta = GetDeltaCameraRotation(DeltaTime, InterpRotationSpeed, OutViewRotation, NewCameraRotation);
			OutViewRotation += CameraDelta;
		}
	}

	return false;
}

void UManaCameraModifier_CamShake::UpdateShake(const float nShakeTime)
{
	ShakeTime = nShakeTime;
}

FRotator UManaCameraModifier_CamShake::GetDeltaCameraRotation(float DeltaTime, float InterpSpeed, const FRotator& CurrentRotation, const FRotator& FinalRotation)
{
	return FMath::RInterpTo(CurrentRotation, FinalRotation, DeltaTime, InterpSpeed) - CurrentRotation;
}