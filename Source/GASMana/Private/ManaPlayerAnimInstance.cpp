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

	if (ManaMovementComponent)
	{
		GroundSpeed = (UKismetMathLibrary::VSizeXY(ManaMovementComponent->Velocity));
		isFalling = ManaMovementComponent->IsFalling();
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
