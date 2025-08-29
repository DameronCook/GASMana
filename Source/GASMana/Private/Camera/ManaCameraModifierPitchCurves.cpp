// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaCameraModifierPitchCurves.h"

#include "PlayerManaCharacter.h"
#include "Camera/ManaPlayerCamManager.h"

UManaCameraModifierPitchCurves::UManaCameraModifierPitchCurves() : PitchToDistanceCurve(nullptr),
                                                                   PitchToFOVCurve(nullptr), PlayerChar(nullptr),
                                                                   CurrentPitchToDist(0),
                                                                   CurrentAddFOV(0)
{
}

bool UManaCameraModifierPitchCurves::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	Super::ModifyCamera(DeltaTime, InOutPOV);

	AManaPlayerCamManager* CamManager = Cast<AManaPlayerCamManager>(CameraOwner);

	if (!IsValid(CamManager))
	{
		return false;
	}

	//Get Camera Location and rotation
	const FVector CamLocation = InOutPOV.Location;
	const FRotator CamRotation = InOutPOV.Rotation;

	//Apply curves
	float TargetPitchToDist = IsValid(PitchToDistanceCurve) ? PitchToDistanceCurve->GetFloatValue(CamRotation.Pitch) : 0.f;
	float AddFOV = IsValid(PitchToFOVCurve) ? PitchToFOVCurve->GetFloatValue(CamRotation.Pitch) : 0.f;

	float InterpSpeed = 5.f;

	if (WallRunning)
	{
		TargetPitchToDist = 50.f;
	}

	if (IsZipToPoint)
	{
		TargetPitchToDist = 100.f;
		InterpSpeed = 5.f;
		AddFOV = 10.f;
	}

	if (IsRoll)
	{
		TargetPitchToDist = 150.f;
		InterpSpeed = 5.f;
		AddFOV = 10.f;
	}

	if (IsBlocking)
	{
		if (PlayerChar)
		{
			if (PlayerChar->GetCombatCameraTarget())
			{
				
				TargetPitchToDist = 150.f;
				InterpSpeed = 15.f;
			}
			else
			{
				TargetPitchToDist = 250.f;
				InterpSpeed = 5.f;
			}
		}
	}

	if (IsAirAttack)
	{
		TargetPitchToDist = 100.f;
		InterpSpeed = 5.f;
		AddFOV = 10.f;
	}

	CurrentPitchToDist = FMath::FInterpTo(CurrentPitchToDist, TargetPitchToDist, DeltaTime, InterpSpeed);
	CurrentAddFOV = FMath::FInterpTo(CurrentAddFOV, AddFOV, DeltaTime, InterpSpeed);

	FVector DesiredLocation = CamLocation - CamRotation.RotateVector(FVector::ForwardVector) * CurrentPitchToDist;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, GetViewTarget());
	FHitResult Result;

	GetWorld()->SweepSingleByChannel(Result, GetViewTarget()->GetActorLocation(), DesiredLocation, FQuat::Identity, CamManager->LineOfSightProbeChannel, FCollisionShape::MakeSphere(CamManager->LineOfSightProbeSize), QueryParams);

	DesiredLocation = Result.bBlockingHit ? Result.Location : DesiredLocation;
	
	InOutPOV.Location = DesiredLocation;
	InOutPOV.FOV += CurrentAddFOV;

	return false;
}
