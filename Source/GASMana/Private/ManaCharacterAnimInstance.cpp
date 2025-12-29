// Fill out your copyright notice in the Description page of Project Settings.


#include "ManaCharacterAnimInstance.h"

// Fill out your copyright notice in the Description page of Project Settings.


#include "ManaPlayerAnimInstance.h"
#include "PlayerManaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"


void UManaCharacterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ManaCharacter = Cast<AGASManaCharacter>(TryGetPawnOwner());
	if (ManaCharacter)
	{
		ManaMovementComponent = ManaCharacter->GetCharacterMovement();
	}
}

void UManaCharacterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (ManaCharacter && ManaMovementComponent)
	{
		GroundSpeed = (UKismetMathLibrary::VSizeXY(ManaMovementComponent->Velocity));
		bIsFalling = ManaMovementComponent->IsFalling();
		LeanAmount = Get_LeanAmount();
		EES = ManaCharacter->GetEquipmentState();
		//GEngine->AddOnScreenDebugMessage(101, 0.1f, FColor::Orange, FString::Printf(TEXT("Equipping: %s"), bIsEquipping ? TEXT("true") : TEXT("false")));

		bIsDead = ManaCharacter->GetIsDead();
		DeathType = ManaCharacter->GetDeathType();
	}
}

void UManaCharacterAnimInstance::SetIsBlocking(bool bBlocking)
{
	bIsBlocking = bBlocking;

	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("IsBlocking: %s"), bIsBlocking ? TEXT("true") : TEXT("false")));
	//}
}


void UManaCharacterAnimInstance::SetIsAttacking(bool bAttacking)
{
	bIsAttacking = bAttacking;

	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("bIsAttacking: %s"), bIsAttacking ? TEXT("true") : TEXT("false")));
	//}
}


void UManaCharacterAnimInstance::SetIsRunning(bool bRunning)
{
	bIsRunning = bRunning;

	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("bIsRunning: %s"), bIsRunning ? TEXT("true") : TEXT("false")));
	//}
}

void UManaCharacterAnimInstance::SetIsEquipping(bool bEquipping)
{
	bIsEquipping = bEquipping;
}

void UManaCharacterAnimInstance::SetIsDead(bool bDead)
{
	bIsDead = bDead;
}

float UManaCharacterAnimInstance::Get_LeanAmount()
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

FVector UManaCharacterAnimInstance::CalculateRelativeAccelerationAmount()
{
	if (ManaCharacter && ManaMovementComponent)
	{
		if (ManaMovementComponent->MaxAcceleration > 0.0f && ManaMovementComponent->GetMaxBrakingDeceleration() > 0.0f)
		{
			const FVector Accel = ManaMovementComponent->GetCurrentAcceleration();
			const FVector Velocity = ManaMovementComponent->Velocity;

			const float MaxAccel = (UKismetMathLibrary::Dot_VectorVector(Accel, Velocity) > 0.0f) ? ManaMovementComponent->GetMaxAcceleration() : ManaMovementComponent->GetMaxBrakingDeceleration();

			const FVector VelAccel = GetVelocityAccel();

			const FVector ClampedVelocity = UKismetMathLibrary::Vector_ClampSizeMax(VelAccel, MaxAccel);
			const FVector DividedVector = (MaxAccel != 0.0f) ? UKismetMathLibrary::Divide_VectorFloat(ClampedVelocity, MaxAccel) : FVector::ZeroVector;

			const FRotator CharRotation = ManaCharacter->GetActorRotation();
			const FVector LocalAccel = CharRotation.UnrotateVector(DividedVector);

			return LocalAccel;
		}
	}
	return FVector();
}

FVector UManaCharacterAnimInstance::GetVelocityAccel()
{
	return ManaMovementComponent->GetCurrentAcceleration();
}
