// Fill out your copyright notice in the Description page of Project Settings.


#include "ManaPlayerAnimInstance.h"
#include "PlayerManaCharacter.h"

void UManaPlayerAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	ManaPlayer = Cast<APlayerManaCharacter>(TryGetPawnOwner());
}

void UManaPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (ManaPlayer)
	{
		if (bShouldStrafe)
		{
			TurnAxis = FMath::RadiansToDegrees(acosf(FVector::DotProduct(ManaPlayer->GetActorForwardVector(), ManaPlayer->GetDirectionVectorToCombatTarget())));
			GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Purple, FString::Printf(TEXT("Turn Axis: %f"), TurnAxis));
		}
	}
}
