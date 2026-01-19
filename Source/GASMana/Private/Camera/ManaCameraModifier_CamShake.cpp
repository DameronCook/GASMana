// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaCameraModifier_CamShake.h"
#include "PlayerManaCharacter.h"
#include "Camera/ManaSpringArmComponent.h"

class UManaSpringArmComponent;

UManaCameraModifier_CamShake::UManaCameraModifier_CamShake()
{
}

bool UManaCameraModifier_CamShake::ProcessViewRotation(class AActor* ViewTarget, float DeltaTime,
	FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	float PitchShake = FMath::Sin(ShakeDecay * ShakeFrequency) * ShakeAmplitude;
	float YawShake   = FMath::Cos(ShakeDecay * ShakeFrequency) * ShakeAmplitude;
	float RollShake  = FMath::Sin(ShakeDecay * ShakeFrequency * 0.5f) * (ShakeAmplitude * 0.5f);

	FRotator ShakeRotator(PitchShake, YawShake, RollShake);
	OutViewRotation += ShakeRotator;
	OutDeltaRot = ShakeRotator;
	
	ShakeDecay -= DeltaTime;
	if (ShakeDecay <= 0)
	{
		ShakeRunning = false;
	}

	return false;
}

void UManaCameraModifier_CamShake::UpdateShake(float nShakeTime, float nTrauma, float nAmplitude, float nFrequency)
{
	ShakeDecay += nShakeTime;
	ShakeAmplitude = nAmplitude;
	ShakeFrequency = nFrequency;
}

FRotator UManaCameraModifier_CamShake::GetDeltaCameraRotation(float DeltaTime, float InterpSpeed, const FRotator& CurrentRotation, const FRotator& FinalRotation)
{
	return FMath::RInterpTo(CurrentRotation, FinalRotation, DeltaTime, InterpSpeed) - CurrentRotation;
}
