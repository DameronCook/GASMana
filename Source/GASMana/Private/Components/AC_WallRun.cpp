// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AC_WallRun.h"
#include "PlayerManaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"

// Sets default values for this component's properties
UAC_WallRun::UAC_WallRun()
{
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UAC_WallRun::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void UAC_WallRun::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(GetOwner());
	UCharacterMovementComponent* CharMove = PlayerCharacter->GetCharacterMovement();
	UAbilitySystemComponent* AbilitySystem = PlayerCharacter->GetAbilitySystemComponent();

	if (!(PlayerCharacter && CharMove && AbilitySystem)) return;

	float CurrentMana = AbilitySystem->GetNumericAttribute(UManaAttributeSet::GetManaAttribute());

	if (CurrentMana > 0)
	{
		bool bIsInAir = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));
		bool bIsWallRunning = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));
		bool bIsWallJumping = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallJumping")));
		bool bIsZipToPoint = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsZipToPoint")));

		if (AbilitySystem && bIsInAir && !bIsWallRunning && !bIsWallJumping && !bIsZipToPoint)
		{
			if (WallRunCheck(PlayerCharacter, CharMove))
			{
				AbilitySystem->TryActivateAbilitiesByTag(PlayerCharacter->GetWallRunTag(), true);
			}
		}
	}

	if (PlayerCharacter && CharMove)
	{
		UGA_ManaPlayerWallRun* WallRunAbility = PlayerCharacter->GetWallRunAbility();
		if (WallRunAbility)
		{
			UpdateWallRunVertical(DeltaTime, PlayerCharacter, WallRunAbility, CharMove);

			PlayerCharacter->SwitchCamaeraState(ECameraState::E_WallRun);

			bool bForwardHit = ForwardWallRunCheck(PlayerCharacter);

			FVector NewWallRunHorizontalDirection = UpdateWallRunHorizontal(PlayerCharacter, WallRunAbility, CharMove);
			CharMove->Velocity.X = NewWallRunHorizontalDirection.X * WallRunStrength;
			CharMove->Velocity.Y = NewWallRunHorizontalDirection.Y * WallRunStrength;

			PlayerCharacter->SetActorRotation(FMath::RInterpTo(PlayerCharacter->GetActorRotation(), NewWallRunHorizontalDirection.Rotation(), DeltaTime, 5.0f));

			if (CurrentMana <= 0.0f || bForwardHit || !CharMove->IsFalling())
			{
				WallRunAbility->OnWallRunFinished(); //This line is suscceptible to crash that's not how you spell that
			}
		}
	}
}


bool UAC_WallRun::WallRunCheck(ACharacter* Character, UCharacterMovementComponent* CharMove)
{

	FHitResult OutHitLine;

	// Line parameters
	const FVector LineStartLocation = Character->GetActorLocation();
	const float LineTraceLength = 40.0f;
	const FVector LineEndLocation = LineStartLocation + (Character->GetActorForwardVector() * LineTraceLength);
	FVector LineOffset = FVector(0.0f, 0.0f, 80.0f);


	//Trace Parameters
	//bool bLineTraceComplex = false;
	FCollisionQueryParams LineQueryParams;
	LineQueryParams.AddIgnoredActor(Character);

	//DrawDebugLine(GetWorld(), LineStartLocation - LineOffset, LineEndLocation - LineOffset, FColor::Red);
	bool bLineHit = GetWorld()->LineTraceSingleByChannel(OutHitLine, LineStartLocation - LineOffset, LineEndLocation - LineOffset, ECollisionChannel::ECC_Visibility, LineQueryParams);

	if (bLineHit)
	{
		return false;
	}

	FHitResult OutHit;

	// Capsule parameters
	const FVector CapsuleLocation = LineStartLocation;
	const float CapsuleRadius = 38.0f;
	const float CapsuleHalfHeight = 40.0f;
	const FQuat CapsuleRotation = FQuat::Identity;

	//Trace Parameters
	bool bTraceComplex = false;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	// Draw the debug capsule
	
	//DrawDebugCapsule(GetWorld(), CapsuleLocation, CapsuleHalfHeight, CapsuleRadius, CapsuleRotation, FColor::Red, false, 0.f);

	bool bHit = GetWorld()->SweepSingleByChannel(OutHit, CapsuleLocation, CapsuleLocation, CapsuleRotation, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight), QueryParams);
	if (bHit)
	{
		//DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 20, FColor::Green, false, 1.0f);
		if (CanWallRunOnSurface(OutHit.ImpactNormal, CharMove))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Can Wall Run on Surface!");
			//Initialize variables based on side of wall here
			if (IsWallRunningAlongRightSide(OutHit.ImpactNormal, Character))
			{
				WallRunSide = EWallRunSide::Right;
				WallRunDir = FVector(0, 0, -1);
				WallRunDirection = SetWallRunDirection(WallRunDir, OutHit.ImpactNormal);
				/*			if (GEngine)
							{
								GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Wall Run along Right!");
							}*/
				PlayableWallRunMontage = WallRunRightMontage;
			}
			else
			{
				WallRunSide = EWallRunSide::Left;
				WallRunDir = FVector(0, 0, 1);
				WallRunDirection = SetWallRunDirection(WallRunDir, OutHit.ImpactNormal);
				/*	if (GEngine)
					{
						GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Wall Run along Left!");
					}*/
				PlayableWallRunMontage = WallRunLeftMontage;
			}

			//Set the variables that need to be reset to start the vertical motion
			WallRunElapsedTime = 0.0f;
			WallRunStartZ = Character->GetActorLocation().Z;
			LastWallRunSineOffset = 0.0f; // If always start at base

			// If you can start above the wall:
			float CurrentZ = Character->GetActorLocation().Z;
			LastWallRunSineOffset = CurrentZ - WallRunStartZ;

			//Start the wall run
			return true;
		}
	}
	return false;
}

bool UAC_WallRun::CanWallRunOnSurface(FVector ImpactNormal, UCharacterMovementComponent* CharMove)
{
	//Calculate if the wall we're on is angled down like an overhang 
	if (ImpactNormal.Z < -.09)
	{
		GEngine->AddOnScreenDebugMessage(-1, .2f, FColor::Red, "Overhang detected! Canceling wall run!");
		return false;
	}

	//Calculate Surface Angle  of the impact point
	FVector XYVector = FVector(ImpactNormal.X, ImpactNormal.Y, 0).GetSafeNormal();				//Gives us the XY Plane of the surface normal. Basically it gives us the horizontal plane of the point
	float DotProduct = FVector::DotProduct(XYVector, ImpactNormal);
	double FloorAngle = acos(DotProduct);														//Gives the angle in radians between the two vectors. So the Dot product returns how different the two planes are. 
	FloorAngle = FMath::RadiansToDegrees(FloorAngle);											//Convert to degrees for later implementation

	//Calculate Angle walkable by the floor
	double WalkableFloorAngle = CharMove->GetWalkableFloorAngle();

	//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Red, FString::Printf(TEXT("Floor Angle: %f"), FloorAngle));
	//GEngine->AddOnScreenDebugMessage(2, 2.f, FColor::Red, FString::Printf(TEXT("WalkableFloor Angle: %f"), WalkableFloorAngle));


	return FloorAngle < WalkableFloorAngle;
}

bool UAC_WallRun::IsWallRunningAlongRightSide(FVector ImpactNormal, ACharacter* Character)
{
	FVector ActorRight = Character->GetActorRightVector();
	double RightVectorDot = ImpactNormal.Dot(ActorRight);

	if (RightVectorDot < 0)
	{
		return true;
	}

	return false;
}

bool UAC_WallRun::ForwardWallRunCheck(ACharacter* Character)
{
	FHitResult OutHit;

	// Line parameters
	const FVector LineStartLocation = Character->GetActorLocation();
	const float LineTraceLength = 40.0f;
	const FVector LineEndLocation = LineStartLocation + (Character->GetActorForwardVector() * LineTraceLength);

	//Trace Parameters
	bool bTraceComplex = false;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	//DrawDebugLine(GetWorld(), LineStartLocation, LineEndLocation, FColor::Red);
	FVector LineOffset = FVector(0.0f, 0.0f, 80.0f);
	bool bHitTop = GetWorld()->LineTraceSingleByChannel(OutHit, LineStartLocation + LineOffset, LineEndLocation + LineOffset, ECollisionChannel::ECC_Visibility, QueryParams);
	bool bHitBottom = GetWorld()->LineTraceSingleByChannel(OutHit, LineStartLocation - LineOffset, LineEndLocation - LineOffset, ECollisionChannel::ECC_Visibility, QueryParams);

	return bHitTop || bHitBottom;
}

FVector UAC_WallRun::SetWallRunDirection(FVector SideMultiplier, FVector ImpactNormal)
{
	return FVector::CrossProduct(ImpactNormal, SideMultiplier);
}

FVector UAC_WallRun::UpdateWallRunHorizontal(APlayerManaCharacter* Character, UGA_ManaPlayerWallRun* WallRunAbility, UCharacterMovementComponent* CharMove)
{
	FHitResult OutHit;

	// Capsule parameters
	const FVector LineStartLocation = Character->GetActorLocation();
	const float LineTraceLength = 75.0f;
	const FVector LineEndLocation = (FVector::CrossProduct(WallRunDirection, WallRunDir) * LineTraceLength) + LineStartLocation;

	//Trace Parameters
	bool bTraceComplex = false;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	//DrawDebugLine(GetWorld(), LineStartLocation, LineEndLocation, FColor::Red);
	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, LineStartLocation, LineEndLocation, ECollisionChannel::ECC_Visibility, QueryParams);

	if (bHit)
	{
		WallRunImpactNormal = OutHit.ImpactNormal;
		WallRunDirection = SetWallRunDirection(WallRunDir, WallRunImpactNormal);
		return WallRunDirection.GetSafeNormal();
	}
	else
	{
		if (WallRunAbility)
		{
			WallRunAbility->OnWallRunFinished();
		}
		CharMove->Velocity = FVector(WallRunDirection.X * WallRunStrength, WallRunDirection.Y * WallRunStrength, CharMove->Velocity.Z);
		return WallRunDirection;
	}
}

void UAC_WallRun::UpdateWallRunVertical(float DeltaTime, APlayerManaCharacter* Character, UGA_ManaPlayerWallRun* WallRunAbility, UCharacterMovementComponent* CharMove)
{
	FHitResult OutHit;

	// Capsule parameters
	const FVector LineStartLocation = Character->GetActorLocation();
	const float LineTraceLength = 100.0f;
	const FVector LineEndLocation = (FVector::CrossProduct(WallRunDirection, WallRunDir) * LineTraceLength) + LineStartLocation;

	//Trace Parameters
	bool bTraceComplex = false;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Character);

	const FVector LineBelowLoc = FVector(0.0f, 0.0f, -80.f);
	const FVector LineAboveLoc = FVector(0.0f, 0.0f, 80.f);

	//DrawDebugLine(GetWorld(), LineStartLocation + LineBelowLoc, LineEndLocation + LineBelowLoc, FColor::Red);
	//DrawDebugLine(GetWorld(), LineStartLocation + LineAboveLoc, LineEndLocation + LineAboveLoc, FColor::Red);
	bool bHitBelow = GetWorld()->LineTraceSingleByChannel(OutHit, LineStartLocation + LineBelowLoc, LineEndLocation + LineBelowLoc, ECollisionChannel::ECC_Visibility, QueryParams);
	bool bHitTop = GetWorld()->LineTraceSingleByChannel(OutHit, LineStartLocation + LineAboveLoc, LineEndLocation + LineAboveLoc, ECollisionChannel::ECC_Visibility, QueryParams);

	WallRunElapsedTime += DeltaTime;
	float Time = WallRunElapsedTime / WallRunDuration;
	if (Time >= 1.0f || !bHitBelow)
	{
		if (WallRunAbility)
		{
			if (GEngine)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Wall Run Ended!");
			}
			WallRunAbility->OnWallRunFinished();
			CharMove->Velocity = FVector(WallRunDirection.X * WallRunStrength, WallRunDirection.Y * WallRunStrength, CharMove->Velocity.Z);
			return;
		}
	}

	float SineOffset = WallRunAmplitude * FMath::Sin(2 * PI * WallRunFreq * Time);


	// Only allow upward movement if bHitTop is true, always allow downward
	bool bMovingUp = SineOffset > LastWallRunSineOffset;
	if (bMovingUp && !bHitTop)
	{
		return;
	}

	FVector CurrentLocation = LineStartLocation;
	CurrentLocation.Z = WallRunStartZ + SineOffset;
	Character->SetActorLocation(CurrentLocation, true);

	// Store for next frame
	LastWallRunSineOffset = SineOffset;
}