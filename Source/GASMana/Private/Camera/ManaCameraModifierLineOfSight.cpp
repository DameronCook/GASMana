// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaCameraModifierLineOfSight.h"

#include "Camera/PlayerCameraManager.h"
#include "Camera/ManaPlayerCamManager.h"

UManaCameraModifierLineOfSight::UManaCameraModifierLineOfSight()
{
	MaxAngleInRadians = PI / 2;
	StepSizeRadians = 0.1f;

	RotationSpeed = 1.0f;

	LineOfSightProbeChannel = ECC_Camera;
}

bool UManaCameraModifierLineOfSight::ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

	if (PlayerHasRecentlyChangedCamera())
	{
		return false;
	}

	const FVector DesiredLocation = CameraOwner->GetCameraLocation();
	const FRotator DesiredRotation = CameraOwner->GetCameraRotation();

	const FVector TargetLocation = GetViewTarget()->GetActorLocation();

	float DesiredAngleInRadians = 0.f;
	float CheckAngleInRadians = 0.f;

	//First we check left
	CheckAngleInRadians = -StepSizeRadians;

	while (CheckAngleInRadians >= -MaxAngleInRadians)
	{
		FVector TestLocation = DesiredLocation;
		FRotator TestRotation = DesiredRotation;

		RotateAroundLocation(TargetLocation, TestLocation, TestRotation, CheckAngleInRadians);

		//Check if line of sight is blocked
		if (!IsInLineOfSight(TargetLocation, TestLocation))
		{
			/*
			* We found an obstacle for this angle
			* The smaller the absolute value of checked angle in radians, the closer the obstacle and the further we need to swing away
			*/
			DesiredAngleInRadians += CheckAngleInRadians + MaxAngleInRadians;
			break;
		}

		//Look further away
		CheckAngleInRadians -= StepSizeRadians;
	}
	
	//Then we check right
	CheckAngleInRadians = StepSizeRadians;

	while (CheckAngleInRadians <= MaxAngleInRadians)
	{
		FVector TestLocation = DesiredLocation;
		FRotator TestRotation = DesiredRotation;

		RotateAroundLocation(TargetLocation, TestLocation, TestRotation, CheckAngleInRadians);

		//Check if line of sight is blocked
		if (!IsInLineOfSight(TargetLocation, TestLocation))
		{
			/*
			* We found an obstacle for this angle
			* The smaller the absolute value of checked angle in radians, the closer the obstacle and the further we need to swing away
			*/
			DesiredAngleInRadians += CheckAngleInRadians - MaxAngleInRadians;
			break;
		}

		//Look further away
		CheckAngleInRadians += StepSizeRadians;
	}

	//Apply Rotation

	const float AppliedAngleInRadians = FMath::Clamp(DesiredAngleInRadians, -RotationSpeed * DeltaTime, +RotationSpeed * DeltaTime);

	OutDeltaRot.Yaw += FMath::RadiansToDegrees(AppliedAngleInRadians);

	return false;
}

bool UManaCameraModifierLineOfSight::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
	Super::ModifyCamera(DeltaTime, InOutPOV);

	return false;
}

bool UManaCameraModifierLineOfSight::IsInLineOfSight(const FVector& From, const FVector& To) const
{
	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return false;
	}

	AManaPlayerCamManager* CamManger = Cast<AManaPlayerCamManager>(CameraOwner);

	if (!IsValid(CamManger))
	{
		return false;
	}

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, GetViewTarget());
	FHitResult Result;
	World->SweepSingleByChannel(Result, From, To, FQuat::Identity, LineOfSightProbeChannel, FCollisionShape::MakeSphere(CamManger->LineOfSightProbeSize), QueryParams);

	return !Result.bBlockingHit;
}

void UManaCameraModifierLineOfSight::RotateAroundLocation(const FVector& TargetLocation, FVector& InOutViewLocation, FRotator& InOutViewRotation, const float AngleInRadians)
{
	FRotator NewRotation = InOutViewRotation;
	NewRotation.Yaw = FMath::RadiansToDegrees(AngleInRadians);
	InOutViewRotation = NewRotation;

	//Rotate Test Location Around Pawn
	FVector NewLocation = InOutViewLocation;

	const float AngleSin = FMath::Sin(AngleInRadians);
	const float AngleCos = FMath::Cos(AngleInRadians);

	NewLocation.X -= TargetLocation.X;
	NewLocation.Y -= TargetLocation.Y;

	//Rotate the point (study this)
	const float RotatedX = NewLocation.X * AngleCos - NewLocation.Y * AngleSin;
	const float RotatedY = NewLocation.X * AngleSin - NewLocation.Y * AngleCos;

	//Translate the point back
	NewLocation.X = RotatedX + TargetLocation.X;
	NewLocation.Y = RotatedY + TargetLocation.Y;

	InOutViewLocation = NewLocation;
}
