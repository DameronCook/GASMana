// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaCameraModifierPlayerActions.h"
#include "Camera/ManaSpringArmComponent.h"
#include "PlayerManaCharacter.h"
#include "Kismet/KismetMathLibrary.h"

UManaCameraModifierPlayerActions::UManaCameraModifierPlayerActions()
{
	RotationSpeed = 10.f;
}

bool UManaCameraModifierPlayerActions::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	return Super::ModifyCamera(DeltaTime, InOutPOV);;
}

bool UManaCameraModifierPlayerActions::ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);
	if (const APlayerManaCharacter* PlayerChar = Cast<APlayerManaCharacter>(ViewTarget))
	{
		FRotator NewCameraRotation = FRotator(OutViewRotation.Pitch, OutViewRotation.Yaw, 0.f);
		FVector TargetCamSocketOffset = FVector(0.f, 40.f, 40.f);
		float InterpRotationSpeed = 3.f;

		if (WallRunning)
		{
			InterpRotationSpeed = 4.f;

			const bool bWallRunRight = (PlayerChar->GetWallRun()->GetWallRunSide() == EWallRunSide::Right);

			TargetCamSocketOffset = bWallRunRight ? FVector(0.f, -150.f, 0.f): FVector(0.f, 150.f, 0.f);

			const float NewRoll = bWallRunRight ? -15.f : 15.f;
			const float NewYaw = bWallRunRight ? -20.f : 20.f;
			constexpr float NewPitch = -10.f;

			NewCameraRotation = PlayerChar->GetActorRotation();

			NewCameraRotation.Roll = NewRoll;
			NewCameraRotation.Yaw += NewYaw;
			NewCameraRotation.Pitch += NewPitch;

			//Strings for debugging
			//GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Red, FString::Printf(TEXT("Roll amount is: %f"), OutViewRotation.Roll));
		}

		if (IsZipToPoint)
		{
			const ITargetingActorInterface* TargetingActor = Cast<ITargetingActorInterface>(ViewTarget);
			const AActor* CurrentTarget = TargetingActor->GetCurrentTarget();

			InterpRotationSpeed = 100.f;

			const FVector CameraLoc = CameraOwner->GetCameraLocation();
			FVector DesiredLoc = PlayerChar->GetActorLocation();
			if (CurrentTarget) DesiredLoc = CurrentTarget->GetActorLocation();
			NewCameraRotation = UKismetMathLibrary::FindLookAtRotation(CameraLoc, DesiredLoc);
		}

			
		if (IsBlocking) {
			if (!IsRunning)
			{
				InterpRotationSpeed = 7.f;
				NewCameraRotation = PlayerChar->GetActorForwardVector().Rotation();
				NewCameraRotation.Pitch -= 5.f;
			}
		}

		if (IsRoll)
		{
			InterpRotationSpeed = 7.f;

			NewCameraRotation = PlayerChar->GetActorForwardVector().Rotation();

			NewCameraRotation.Pitch -= 15.f;
		}

		if (IsAirAttack)
		{
			InterpRotationSpeed = 7.f;
			NewCameraRotation = PlayerChar->GetActorForwardVector().Rotation();
			NewCameraRotation.Pitch -= 25.f;

			TargetCamSocketOffset = FVector(0.f, 150.f, 0.f);

		}

		
		if (IsFocused)
		{
			if (const AActor* CurrentTarget = PlayerChar->GetCombatCameraTarget())
			{
				InterpRotationSpeed = 8.f;
				const FVector CameraLoc = CameraOwner->GetCameraLocation();
				const FRotator DesiredCamRot = UKismetMathLibrary::FindLookAtRotation(CameraLoc, CurrentTarget->GetActorLocation());
				NewCameraRotation = FRotator(0.f, DesiredCamRot.Yaw, 0.f);
			}
		}

		UManaSpringArmComponent* PlayerSpringArm = PlayerChar->GetCameraBoom();
		PlayerSpringArm->SocketOffset = FMath::VInterpTo(PlayerSpringArm->SocketOffset, TargetCamSocketOffset, DeltaTime, InterpRotationSpeed);

		const FRotator CameraDelta = GetDeltaCameraRotation(DeltaTime, InterpRotationSpeed, OutViewRotation, NewCameraRotation);
		OutViewRotation += CameraDelta;
	}
	return false;
}

FRotator UManaCameraModifierPlayerActions::GetDeltaCameraRotation(float DeltaTime, float InterpSpeed, const FRotator& CurrentRotation, const FRotator& FinalRotation)
{
    return FMath::RInterpTo(CurrentRotation, FinalRotation, DeltaTime, InterpSpeed) - CurrentRotation;
}

FVector UManaCameraModifierPlayerActions::GetDeltaCameraLocation(const float DeltaTime, float InterpSpeed, const FVector& CurrentLocation, const FVector& FinalLocation)
{
	return FMath::VInterpTo(CurrentLocation, FinalLocation, DeltaTime, InterpSpeed) - CurrentLocation;
}