// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WinMenu.generated.h"

class UBorder;
class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class GASMANA_API UWinMenu : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;

protected:
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* WinTitleText;
	
	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* WinFlavorText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UButton* RestartButton;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UButton* QuitButton;
	
	UPROPERTY(Transient, BlueprintReadOnly, meta = (BindWidgetAnim))
	UWidgetAnimation* StartAnim;

	UFUNCTION()
	void RestartClicked();

	UFUNCTION()
	void QuitClicked();

public:
	FORCEINLINE UWidgetAnimation* GetStartAnimation() const { return StartAnim; }
};
