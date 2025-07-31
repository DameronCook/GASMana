// Fill out your copyright notice in the Description page of Project Settings.


#include "ManaPlayerAnimInstance.h"
#include "PlayerManaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UManaPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ManaCharacter = Cast<APlayerManaCharacter>(TryGetPawnOwner());
	if (ManaCharacter)
	{
		ManaMovementComponent = ManaCharacter->GetCharacterMovement();
	}
}

void UManaPlayerAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (ManaCharacter && ManaMovementComponent)
	{
		GroundSpeed = (UKismetMathLibrary::VSizeXY(ManaMovementComponent->Velocity));
		isFalling = ManaMovementComponent->IsFalling();
		LeanAmount = Get_LeanAmount();
		EES = ManaCharacter->GetEquipmentState();
		//GEngine->AddOnScreenDebugMessage(5, 0.1f, FColor::Orange, FString::Printf(TEXT("Lean Amount: % f"), LeanAmount));

		//Do this AFTER lean
		//VelocityLastFrame = ManaMovementComponent->GetLastUpdateVelocity();
	}
}

void UManaPlayerAnimInstance::SetIsBlocking(bool bBlocking)
{
	bIsBlocking = bBlocking;

	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("IsBlocking: %s"), bIsBlocking ? TEXT("true") : TEXT("false")));
	//}
}


void UManaPlayerAnimInstance::SetIsAttacking(bool bAttacking)
{
	bIsAttacking = bAttacking;

	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("bIsAttacking: %s"), bIsAttacking ? TEXT("true") : TEXT("false")));
	//}
}


void UManaPlayerAnimInstance::SetIsRunning(bool bRunning)
{
	bIsRunning = bRunning;

	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("bIsRunning: %s"), bIsRunning ? TEXT("true") : TEXT("false")));
	//}
}

float UManaPlayerAnimInstance::Get_LeanAmount()
{
	float fLeanAmount = 0.0f;

	if (ManaMovementComponent)
	{
		FVector RelativeAccel = CalculateRelativeAccelerationAmount();
		float Speed = UKismetMathLibrary::VSizeXY(ManaMovementComponent->Velocity);
		float ClampedSpeed = UKismetMathLibrary::MapRangeClamped(Speed, 0.f, 100.f, 0.75f, 1.35f);
		fLeanAmount = RelativeAccel.Y * ClampedSpeed;
		fLeanAmount = FMath::Clamp(fLeanAmount, -1.0f, 1.0f);
	}
	return fLeanAmount;
}

FVector UManaPlayerAnimInstance::CalculateRelativeAccelerationAmount()
{
	if (ManaCharacter && ManaMovementComponent)
	{
		float MaxAccel = 0.0f;
		if (ManaMovementComponent->MaxAcceleration > 0.0f && ManaMovementComponent->GetMaxBrakingDeceleration() > 0.0f)
		{
			FVector Accel = ManaMovementComponent->GetCurrentAcceleration();
			FVector Velocity = ManaMovementComponent->Velocity;

			MaxAccel = (UKismetMathLibrary::Dot_VectorVector(Accel, Velocity) > 0.0f) ? ManaMovementComponent->GetMaxAcceleration() : ManaMovementComponent->GetMaxBrakingDeceleration();

			FVector VelAccel = GetVelocityAccel();

			FVector ClampedVelocity = UKismetMathLibrary::Vector_ClampSizeMax(VelAccel, MaxAccel);
			FVector DividedVector = (MaxAccel != 0.0f) ? UKismetMathLibrary::Divide_VectorFloat(ClampedVelocity, MaxAccel) : FVector::ZeroVector;

			FRotator CharRotation = ManaCharacter->GetActorRotation();
			FVector LocalAccel = CharRotation.UnrotateVector(DividedVector);

			//GEngine->AddOnScreenDebugMessage(301, 0.1f, FColor::Cyan, FString::Printf(TEXT("DividedVector: %s"), *DividedVector.ToString()));
			//GEngine->AddOnScreenDebugMessage(302, 0.1f, FColor::Cyan, FString::Printf(TEXT("LocalAccel: %s"), *LocalAccel.ToString()));
			//GEngine->AddOnScreenDebugMessage(303, 0.1f, FColor::Cyan, FString::Printf(TEXT("ClampedSpeed: %s"), *ClampedVelocity.ToString()));
			//GEngine->AddOnScreenDebugMessage(304, 0.1f, FColor::Cyan, FString::Printf(TEXT("RelativeAccel.Y: %f"), LocalAccel.Y));

			return LocalAccel;
		}
	}
	return FVector();
}

FVector UManaPlayerAnimInstance::GetVelocityAccel()
{
	return ManaMovementComponent->GetCurrentAcceleration();
}
