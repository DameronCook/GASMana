// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaSpringArmComponent.h"

#include "CollisionQueryParams.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

UManaSpringArmComponent::UManaSpringArmComponent()
{
	bUsePawnControlRotation = true;

	BaseTargetArmLength = 500.f;
	SocketOffset = FVector(0.f, 40.f, 40.f);

	bEnableCameraLag = true;
	CameraLagSpeed = 5.f;
}

void UManaSpringArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	TargetArmLength = BaseTargetArmLength + TargetArmLengthModifier;
	Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);
	TargetArmLengthModifier = 0.0f;
}
