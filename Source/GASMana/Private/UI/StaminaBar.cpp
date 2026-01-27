// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StaminaBar.h"

void UStaminaBar::NativeConstruct()
{
	Super::NativeConstruct();
	BindToAnimationFinished(FadeOutAnimation, FadeOutAnimEndedDelegate);
	FadeOutAnimEndedDelegate.BindDynamic(this, &UStaminaBar::FadeOutAnimEnded);
}

void UStaminaBar::FadeOutAnimEnded()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Fade out anim ended fired!");
}
