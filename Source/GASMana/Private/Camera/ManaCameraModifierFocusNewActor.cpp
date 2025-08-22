// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaCameraModifierFocusNewActor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Interface/TargetingActorInterface.h"

UManaCameraModifierFocusNewActor::UManaCameraModifierFocusNewActor()
{
	RotationSpeed = 300.f;
	SnapSpeed = 50.f;
}

bool UManaCameraModifierFocusNewActor::ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

	//Check if we're selecting a target
	if (!IsValid(ViewTarget))
	{
		//GEngine->AddOnScreenDebugMessage(1, .1f, FColor::Orange, "No view target");
		return false;
	}

	ITargetingActorInterface* TargetingActor = Cast<ITargetingActorInterface>(ViewTarget);
	
	if (!TargetingActor)
	{
		//GEngine->AddOnScreenDebugMessage(1, .1f, FColor::Orange, "No Target Actor");

		return false;
	}
			
	if (!TargetingActor->IsSelectingTarget())
	{
		//GEngine->AddOnScreenDebugMessage(1, .1f, FColor::Orange, "Not selecting target");
		return false;
	}

	AActor* CurrentTarget = TargetingActor->GetCurrentTarget();

	if (IsValid(CurrentTarget))
	{
		//Do we have a new target?
		if (CurrentTarget != LastTarget)
		{
			//Calculate our desired and current location, rotation
			FVector CameraLoc = CameraOwner->GetCameraLocation();
			FVector DesiredLoc = CurrentTarget->GetActorLocation();

			// Get rotation delta from current camera rotation to look-at rotation for the target. -> Get the change in rotation from our current rotation to our desired rotation
			// As the camera position might change depending on its rotation (e.g. for spring arms), -> it do that.
			// we just compute this once per target and store the result. -> We just need to find that change
			// Otherwise, for very close targets, we risk rotating the camera back and forth as its position changes.
			DesiredRotation = UKismetMathLibrary::FindLookAtRotation(CameraLoc, DesiredLoc);
			LastTarget = CurrentTarget;
		}

		FRotator CurrentRotation = OutViewRotation;

		if (SnapSpeed <= 0)
		{
			//Snap immediately
			OutViewRotation = DesiredRotation;
		}
		else
		{
			FRotator TowardsDesired = DesiredRotation - CurrentRotation;
			FRotator TowardsDesiredNormalized = TowardsDesired.GetNormalized();

			float CurrentRotationSimilarity = FVector::DotProduct(CurrentRotation.Vector(), DesiredRotation.Vector());

			//GEngine->AddOnScreenDebugMessage(2, .1f, FColor::Red, FString::Printf(TEXT("Angle Towards Desired: %f"), CurrentRotationSimilarity));
			//Are we nearly the same?
			if (TowardsDesiredNormalized.IsNearlyZero())
			{
				//Then don't overshoot
				OutViewRotation = DesiredRotation;
			}
			else
			{
				if (CurrentRotationSimilarity < .95f)
				{
					//Otherwise, Apply the rot speed
					FRotator DeltaRot = TowardsDesiredNormalized * SnapSpeed * DeltaTime;
					OutDeltaRot += DeltaRot;
				}
			}
		}
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(1, .1f, FColor::Orange, "No current target");

		LastTarget = nullptr;

		/*
		//No new target applied, Smoothly apply player input
		FVector2D TargetSelectionInput = TargetingActor->GetCurrentTargetSelectionInput();

		FRotator DeltaRot;
		DeltaRot.Yaw = TargetSelectionInput.X * RotationSpeed * DeltaTime;
		DeltaRot.Pitch = TargetSelectionInput.Y * RotationSpeed * DeltaTime;
		DeltaRot.Roll = 0.0f;
		*/
	}

	//This will prevent further modifiers from being applied... careful
	return false;
}
