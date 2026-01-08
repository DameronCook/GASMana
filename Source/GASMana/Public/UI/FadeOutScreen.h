// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FadeOutScreen.generated.h"

class UBorder;
/**
 * 
 */
UCLASS()
class GASMANA_API UFadeOutScreen : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	
	/** Function to start the fade */
	UFUNCTION(Category = "Fade")
	void StartFadeToBlack();

	/** Function to end the fade */
	UFUNCTION(Category = "Fade")
	void FadeToNothing();

	UFUNCTION()
	void FadeOutStarted();
	
	UFUNCTION()
	void FadeOutEnded();
	
	UFUNCTION()
	void FadeInStarted();
	
	UFUNCTION()
	void FadeInEnded();
	
protected:
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UBorder* FadeScreen;

    UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutAnim;
	
    UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInAnim;
	
	FWidgetAnimationDynamicEvent FadeOutStartDelegate;
	FWidgetAnimationDynamicEvent FadeOutEndDelegate;
	
	FWidgetAnimationDynamicEvent FadeInStartDelegate;
	FWidgetAnimationDynamicEvent FadeInEndDelegate;

public:
	FORCEINLINE UWidgetAnimation* GetFadeInAnim() const { return FadeInAnim; }
	FORCEINLINE FWidgetAnimationDynamicEvent GetFadeInAnimStartDelegate() const { return FadeInStartDelegate; }
	FORCEINLINE FWidgetAnimationDynamicEvent GetFadeInAnimEndDelegate() const { return FadeInEndDelegate; }
	FORCEINLINE UWidgetAnimation* GetFadeOutAnim() const { return FadeOutAnim; }
	FORCEINLINE FWidgetAnimationDynamicEvent GetFadeOutAnimStartDelegate() const { return FadeOutStartDelegate; }
	FORCEINLINE FWidgetAnimationDynamicEvent GetFadeOutAnimEndDelegate() const { return FadeOutEndDelegate; }
	
};
