// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AdvancedCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"
#include "PlayerManaCharacter.h"
#include "Actors/ManaHookParent.h"

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

	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(GetOwner());
	if (!PlayerCharacter) return;


	UAbilitySystemComponent* AbilitySystem = PlayerCharacter->GetAbilitySystemComponent();


	//Manually handle camera while player "Is Free"
	bool bIsFree = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.IsFree")));

	if (bIsFree)
	{
		bool bIsBlocking = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsBlocking")));
		bool bIsRunning = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.IsRunning")));

		CamState = ECameraState::E_Default;

		if (bIsBlocking && !bIsRunning)
		{
			CamState = ECameraState::E_Shield;
		}
	}

	//If the player is not free, check if the player has any of these abilities and switch to them
	if (IsValid(PlayerCharacter->GetZipAbility()))
	{
		CamState = ECameraState::E_ZipToPoint;
	}

	if (IsValid(PlayerCharacter->GetSwingAbility()))
	{
		CamState = ECameraState::E_Swing;
	}

	if (IsValid(PlayerCharacter->GetWallRunAbility()))
	{
		CamState = ECameraState::E_WallRun;
	}

	//Switch camera behavior depending on the state
	switch (CamState)
	{
	case ECameraState::E_Default:
		SetDefaultCameraState(PlayerCharacter);
		break;
	case ECameraState::E_ZipToPoint:
		SetZipToPointCameraState(PlayerCharacter);
		break;
	case ECameraState::E_Roll:
		SetRollCameraState(PlayerCharacter);
		break;
	case ECameraState::E_Shield:
		SetShieldCameraState(PlayerCharacter);
		break;
	case ECameraState::E_Swing:
		SetSwingCameraState(PlayerCharacter);
		break;
	case ECameraState::E_WallJump:
		SetWallJumpCameraState(PlayerCharacter->GetWallRun(), PlayerCharacter);
		break;
	case ECameraState::E_WallRun:
		SetWallRunCameraState(PlayerCharacter->GetWallRun(), PlayerCharacter);
		break;
	default:
		SetDefaultCameraState(PlayerCharacter);
		break;
	}

	//This should be the LAST Step after we set our target arm length, rotation, and field of view
	UpdateCameraLengthRotationFOV(DeltaTime, PlayerCharacter);
	
}

void UAdvancedCameraComponent::UpdateCameraLengthRotationFOV(float DeltaTime, APlayerManaCharacter* PlayerCharacter)
{
	/*
	UCameraComponent* PlayerCamera = PlayerCharacter->GetFollowCamera();
	USpringArmComponent* CamSpringArm = PlayerCharacter->GetCameraBoom();
	AController* Controller = PlayerCharacter->GetController();
	if (!CamSpringArm || !PlayerCamera || !Controller) return;

	// Interpolate arm length
	float NewArmLength = FMath::FInterpTo(CamSpringArm->TargetArmLength, TargetCameraState.TargetArmLength, DeltaTime, CameraInterpSpeed);
	//CamSpringArm->TargetArmLength = NewArmLength;

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
	*/
}

void UAdvancedCameraComponent::SetCameraState(const FCameraState& NewCameraState, float InterpSpeed)
{
	TargetCameraState = NewCameraState;
	CameraInterpSpeed = InterpSpeed;
	ChangeCamera = true;
}

//////////////////// -- Camera States -- \\\\\\\\\\\\\\\\\\\\\\\

void UAdvancedCameraComponent::SetDefaultCameraState(APlayerManaCharacter* PlayerCharacter)
{

	// Restore normal camera control
	//CameraBoom->bUsePawnControlRotation = true;
	//CameraBoom->SetRelativeRotation(FRotator::ZeroRotator);
	PlayerCharacter->GetCameraBoom()->bDoCollisionTest = true;

	// In PlayerManaCharacter.cpp, when wall run ends:
	FCameraState DefaultState;
	DefaultState.TargetArmLength = 400.f;
	DefaultState.CameraRotation = FRotator(PlayerCharacter->GetControlRotation().Pitch, PlayerCharacter->GetControlRotation().Yaw, 0.f);
	DefaultState.CameraFOV = 90.f;

	SetCameraState(DefaultState, 5.f);
}

void UAdvancedCameraComponent::SetZipToPointCameraState(APlayerManaCharacter* PlayerCharacter)
{
	//CameraBoom->bUsePawnControlRotation = false;
	PlayerCharacter->GetCameraBoom()->bDoCollisionTest = false;


	FRotator AngleToTarget = PlayerCharacter->GetActorForwardVector().Rotation();

	if (PlayerCharacter->GetHookShot() && PlayerCharacter->GetHookShot()->GetCurrentTarget())
	{
		FVector DirectionToTarget = (PlayerCharacter->GetHookShot()->GetCurrentTarget()->GetActorLocation() - PlayerCharacter->GetActorLocation()).GetSafeNormal();
		AngleToTarget = DirectionToTarget.Rotation();
	}

	FRotator ZipToPointCameraRotation = AngleToTarget;

	FCameraState ZipToPointState;
	ZipToPointState.TargetArmLength = 1.f;
	ZipToPointState.CameraRotation = ZipToPointCameraRotation;
	ZipToPointState.CameraFOV = 100.f;

	SetCameraState(ZipToPointState, 4.f);
}

void UAdvancedCameraComponent::SetSwingCameraState(APlayerManaCharacter* PlayerCharacter)
{
	PlayerCharacter->GetCameraBoom()->bDoCollisionTest = true;
	FRotator AngleToTarget = PlayerCharacter->GetActorForwardVector().Rotation();

	// In PlayerManaCharacter.cpp, when wall run ends:
	FCameraState SwingState;
	SwingState.TargetArmLength = 750.f;
	SwingState.CameraRotation = AngleToTarget;//FRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.f);
	SwingState.CameraFOV = 90.f;

	SetCameraState(SwingState, 20.f);
}

void UAdvancedCameraComponent::SetWallRunCameraState(UAC_WallRun* WallRun, APlayerManaCharacter* PlayerCharacter)
{
	//CameraBoom->bUsePawnControlRotation = false;
	PlayerCharacter->GetCameraBoom()->bDoCollisionTest = false;

	float newRoll = (WallRun->GetWallRunSide() == EWallRunSide::Right) ? -15.f : 15.f;
	float newYaw = (WallRun->GetWallRunSide() == EWallRunSide::Right) ? 10.f : -10.f;
	float newPitch = 0.f;

	FRotator WallRunCameraRotation = PlayerCharacter->GetActorForwardVector().Rotation();

	WallRunCameraRotation.Pitch += newPitch;
	WallRunCameraRotation.Yaw += newYaw;
	WallRunCameraRotation.Roll += newRoll;

	FCameraState WallRunState;
	WallRunState.TargetArmLength = 125.f;
	WallRunState.CameraRotation = WallRunCameraRotation;
	WallRunState.CameraFOV = 110.f;

	SetCameraState(WallRunState, 5.f);
}

void UAdvancedCameraComponent::SetRollCameraState(APlayerManaCharacter* PlayerCharacter)
{
	FRotator RollCameraRotation = PlayerCharacter->GetActorForwardVector().Rotation();

	RollCameraRotation.Pitch -= 15.f;
	RollCameraRotation.Roll = 0.f;

	FCameraState RollState;
	RollState.TargetArmLength = 150.f;
	RollState.CameraRotation = RollCameraRotation;
	RollState.CameraFOV = 100.f;

	SetCameraState(RollState, 7.f);
}

void UAdvancedCameraComponent::SetWallJumpCameraState(UAC_WallRun* WallRun, APlayerManaCharacter* PlayerCharacter)
{
	FRotator JumpCameraRotation = PlayerCharacter->GetActorForwardVector().Rotation();

	JumpCameraRotation.Yaw += (WallRun->GetWallRunSide() == EWallRunSide::Right) ? -90.f : 90.f;
	JumpCameraRotation.Roll = 0.f;

	FCameraState JumpState;
	JumpState.TargetArmLength = 10.f;
	JumpState.CameraRotation = JumpCameraRotation;
	JumpState.CameraFOV = 120.f;

	SetCameraState(JumpState, 20.f);
}

void UAdvancedCameraComponent::SetShieldCameraState(APlayerManaCharacter* PlayerCharacter)
{
	FRotator ShieldCamRot = PlayerCharacter->GetActorForwardVector().Rotation();

	ShieldCamRot.Pitch -= 5.f;
	ShieldCamRot.Roll = 0.f;


	FCameraState ShieldState;
	ShieldState.TargetArmLength = 250.f;
	ShieldState.CameraRotation = ShieldCamRot;
	ShieldState.CameraFOV = 90.f;

	SetCameraState(ShieldState, 10.f);
}

/*
void UAdvancedCameraComponent::PushForceAwayFromWalls(FVector PushAwayDirection, APlayerManaCharacter* PlayerCharacter, UCameraComponent* Cam)
{
	if (!PlayerCharacter || !Cam) return;

	FHitResult OutLineHit;

	// Line parameters
	const FVector LineStartLocation = Cam->GetComponentLocation();
	const float LineTraceLength = 300.f;
	const FVector LineEndLocation = LineStartLocation + (PushAwayDirection * LineTraceLength);

	//Trace Parameters
	FCollisionQueryParams LineQueryParams;
	LineQueryParams.AddIgnoredActor(PlayerCharacter);

	DrawDebugLine(GetWorld(), LineStartLocation, LineEndLocation, FColor::Red);
	bool bBottomLineHit = GetWorld()->LineTraceSingleByChannel(OutLineHit, LineStartLocation, LineEndLocation, ECollisionChannel::ECC_Visibility, LineQueryParams);

	if (bBottomLineHit)
	{
		//GEngine->AddOnScreenDebugMessage(8, 0.1f, FColor::Purple, "I should be OFFSETTING THE PLAYER Z AAAAAAAAAA");
		float AddOffset = UKismetMathLibrary::MapRangeClamped(OutLineHit.Distance, 0.f, LineTraceLength, 25.f, 1.f);

		PlayerCharacter->AddActorWorldOffset(FVector(0.f, 0.f, AddOffset));
	}
}
*/