// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaCameraModifierFocusWalkDir.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/PlayerCharacterInterface.h"

UManaCameraModifierFocusWalkDir::UManaCameraModifierFocusWalkDir()
{
	RotationSpeed = 10.f;
	RotateOnlyWhileCharacterIsMoving = true;
}

bool UManaCameraModifierFocusWalkDir::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	return Super::ModifyCamera(DeltaTime, InOutPOV);
}

bool UManaCameraModifierFocusWalkDir::ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

	if (DirectionChangeCooldownRemaining > 0.f)
	{
		DirectionChangeCooldownRemaining -= DeltaTime;
	}

	IPlayerCharacterInterface* PlayerChar = Cast<IPlayerCharacterInterface>(ViewTarget);

	if (!PlayerChar)
	{
		return false;
	}

	if (RotateOnlyWhileCharacterIsMoving && !PlayerChar->GotMovementInput())
	{
		return false;
	}

	if (PlayerHasRecentlyChangedCamera())
	{
		return false;
	}

	if (const ACharacter* Char = Cast<ACharacter>(ViewTarget))
	{
		FVector PlayerMovementDir = Char->GetCharacterMovement()->Velocity.GetSafeNormal();
		FVector CameraFacingDir = OutViewRotation.Vector();
		float Dot = FVector::DotProduct(PlayerMovementDir, CameraFacingDir);
		if (Dot < -0.75f)
		{
			return false;
		}
	}
	const float ActorYaw = ViewTarget->GetActorRotation().Yaw;
	const float ViewYaw = OutViewRotation.Yaw;

	float YawDelta = ActorYaw - ViewYaw;

	while (YawDelta < -180.0f)
	{
		YawDelta += 360.0f;
	}

	while (YawDelta > 180.0f)
	{
		YawDelta -= 360.0f;
	}

	float YawDeltaSign = FMath::Sign(YawDelta);

	if (PreviousYawDeltaSign != YawDeltaSign)
	{
		if (DirectionChangeCooldownRemaining > 0)
		{
			return false;
		}
		PreviousYawDeltaSign = YawDeltaSign;
		DirectionChangeCooldownRemaining = DirectionChangeCooldown;
	}

	float AppliedYawDelta = YawDelta * RotationSpeed * DeltaTime;

	if (FMath::Abs(YawDelta) < FMath::Abs(AppliedYawDelta))
	{
		PreviousYawDeltaSign = 0.f;
		DirectionChangeCooldownRemaining = DirectionChangeCooldown;
		AppliedYawDelta = YawDelta;
	}

	OutDeltaRot.Yaw = AppliedYawDelta;
		
	return false;
}
