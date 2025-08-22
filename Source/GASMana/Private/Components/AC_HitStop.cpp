// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AC_HitStop.h"
#include "GASMana/GASManaCharacter.h"

// Sets default values for this component's properties
UAC_HitStop::UAC_HitStop()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UAC_HitStop::StartHitStop(float Duration)
{
	if (AGASManaCharacter* GASManaCharacter = Cast<AGASManaCharacter>(GetOwner()))
	{
		GetOwner()->CustomTimeDilation = 0.0f;
		GetWorld()->GetTimerManager().SetTimer(HitStopTimerHandle, this, &UAC_HitStop::EndHitStop, Duration);
	}

}

void UAC_HitStop::EndHitStop() const
{
	GetOwner()->CustomTimeDilation = 1.0f;
}
