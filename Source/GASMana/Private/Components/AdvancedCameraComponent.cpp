// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AdvancedCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "PlayerManaCharacter.h"

UAdvancedCameraComponent::UAdvancedCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	ChangeCamera = false;

	SetTickGroup(ETickingGroup::TG_PrePhysics);
}


void UAdvancedCameraComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UAdvancedCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdateCameraLengthRotationFOV(DeltaTime);
}

void UAdvancedCameraComponent::UpdateCameraLengthRotationFOV(float DeltaTime)
{
	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(GetOwner());
	if (!PlayerCharacter) return;

	UCameraComponent* PlayerCamera = PlayerCharacter->GetFollowCamera();
	USpringArmComponent* CamSpringArm = PlayerCharacter->GetCameraBoom();
	AController* Controller = PlayerCharacter->GetController();
	if (!CamSpringArm || !PlayerCamera || !Controller) return;

	// Interpolate arm length
	float NewArmLength = FMath::FInterpTo(CamSpringArm->TargetArmLength, TargetCameraState.TargetArmLength, DeltaTime, CameraInterpSpeed);
	CamSpringArm->TargetArmLength = NewArmLength;

	// Interpolate FOV
	float NewFOV = FMath::FInterpTo(PlayerCamera->FieldOfView, TargetCameraState.CameraFOV, DeltaTime, CameraInterpSpeed);
	PlayerCamera->SetFieldOfView(NewFOV);

	// --- Blend player input with wall run offset ---
	FRotator TargetOffset = TargetCameraState.CameraRotation;
	FRotator CurrentOffset = Controller->GetControlRotation();
	FRotator NewOffset = FMath::RInterpTo(CurrentOffset, TargetOffset, DeltaTime, CameraInterpSpeed);
	
	Controller->SetControlRotation(NewOffset);

	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Blue, FString("Updating Camera Pos"));
	//}
}

void UAdvancedCameraComponent::SetCameraState(const FCameraState& NewCameraState, float InterpSpeed)
{
	TargetCameraState = NewCameraState;
	CameraInterpSpeed = InterpSpeed;
	ChangeCamera = true;
}

