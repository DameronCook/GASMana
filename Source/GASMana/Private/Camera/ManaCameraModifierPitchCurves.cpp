// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaCameraModifierPitchCurves.h"
#include "Camera/ManaPlayerCamManager.h"

UManaCameraModifierPitchCurves::UManaCameraModifierPitchCurves()
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
	const float PitchToDist = IsValid(PitchToDistanceCurve) ? PitchToDistanceCurve->GetFloatValue(CamRotation.Pitch) : 0.f;
	const float PitchToFOV = IsValid(PitchToFOVCurve) ? PitchToFOVCurve->GetFloatValue(CamRotation.Pitch) : 0.f;

	const FVector DesiredLocation = CamLocation - CamRotation.RotateVector(FVector::ForwardVector) * PitchToDist;

	FVector NewLocation;
	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, GetViewTarget());
	FHitResult Result;

	GetWorld()->SweepSingleByChannel(Result, GetViewTarget()->GetActorLocation(), DesiredLocation, FQuat::Identity, CamManager->LineOfSightProbeChannel, FCollisionShape::MakeSphere(CamManager->LineOfSightProbeSize), QueryParams);

	NewLocation = Result.bBlockingHit ? Result.Location : DesiredLocation;


	//GEngine->AddOnScreenDebugMessage(7, .1f, FColor::Purple, DesiredLocation.ToString());
	//GEngine->AddOnScreenDebugMessage(8, .1f, FColor::Purple, FString::Printf(TEXT("Desired length based on pitch: %f"), PitchToDist));
	//GEngine->AddOnScreenDebugMessage(9, .1f, FColor::Purple, FString::Printf(TEXT("Actual Current Length: %f"), CamManager->));

	InOutPOV.Location = NewLocation;
	InOutPOV.FOV += PitchToFOV;

	return false;
}
