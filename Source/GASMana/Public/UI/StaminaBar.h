// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StackedProgressBar.h"
#include "Blueprint/UserWidget.h"
#include "StaminaBar.generated.h"

class UProgressBar;
/**
 * 
 */
UCLASS()
class GASMANA_API UStaminaBar : public UStackedProgressBar
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutAnimation;
	
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInAnimation;

	FWidgetAnimationDynamicEvent FadeOutAnimEndedDelegate;

	UFUNCTION()
	void FadeOutAnimEnded();

public:
	FORCEINLINE UWidgetAnimation* GetStartAnimation() const { return FadeInAnimation; }
	FORCEINLINE UWidgetAnimation* GetEndAnimation() const { return FadeOutAnimation; }
	FORCEINLINE FWidgetAnimationDynamicEvent GetFadeOutAnimEndedDelegate() const { return FadeOutAnimEndedDelegate; }
};
