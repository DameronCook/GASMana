// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenu.generated.h"

class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class GASMANA_API UPauseMenu : public UUserWidget
{
	GENERATED_BODY()
	
	virtual void NativeConstruct() override;
	
protected:

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* PauseText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UButton* RestartButton;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UButton* QuitButton;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UButton* ResumeButton;

	UPROPERTY(Transient, BlueprintReadOnly, meta=(BindWidgetAnim))
	UWidgetAnimation* PauseStartedAnimation;

	FWidgetAnimationDynamicEvent PauseAnimEndedDelegate;

	UFUNCTION()
	void RestartClicked();

	UFUNCTION()
	void QuitClicked();
	
	UFUNCTION()
	void ResumeClicked();

	UFUNCTION()
	void PauseAnimEnded();

public:

	FORCEINLINE UWidgetAnimation* GetPauseAnimation() const { return PauseStartedAnimation; }
	FORCEINLINE FWidgetAnimationDynamicEvent GetPauseAnimEndedDelegate() const { return PauseAnimEndedDelegate; }
};
