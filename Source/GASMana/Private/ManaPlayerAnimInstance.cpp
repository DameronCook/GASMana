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
			const FVector ActorForward = ManaPlayer->GetActorForwardVector();
			const FVector ToFocus = ManaPlayer->GetDirectionVectorToCombatTarget();
			float TurnAngle = FMath::RadiansToDegrees(acosf(FVector::DotProduct(ActorForward, ToFocus)));
			const FVector CrossProduct = FVector::CrossProduct(ActorForward, ToFocus);

			if (CrossProduct.Z < 0)
			{
				TurnAngle *= -1.f;
			}

			TurnAxis = TurnAngle;
			GEngine->AddOnScreenDebugMessage(5, 1.f, FColor::Purple, FString::Printf(TEXT("Turn Axis: %f"), TurnAxis));
		}
	}
}
