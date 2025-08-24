// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/HitStopInterface.h"

// Add default functionality here for any IHitStopInterface functions that are not pure virtual.
void IHitStopInterface::StartHitStop(float Duration, AActor* HitStopActor)
{
	if (HitStopActor)
	{
		HitStopActor->CustomTimeDilation = 0.0f;

		//Create a delegate that binds the function end hit stop with the actor we're applying the hit stop to.
		FTimerDelegate HitStopDelegate;
		HitStopDelegate.BindLambda([this, HitStopActor]()
		{
			EndHitStop(HitStopActor);
		});
		HitStopActor->GetWorld()->GetTimerManager().SetTimer(HitStopTimerHandle, HitStopDelegate, Duration, false);
	}
}

void IHitStopInterface::EndHitStop(AActor* HitStopActor)
{
	HitStopActor->CustomTimeDilation = 1.0f;
}
