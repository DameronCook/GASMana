// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DeathMenu.generated.h"

class UButton;
class UTextBlock;
/**
 * 
 */
UCLASS()
class GASMANA_API UDeathMenu : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativeConstruct() override;
	
protected:

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* DeathText;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UButton* RestartButton;

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UButton* QuitButton;

	UFUNCTION()
	void RestartClicked();

	UFUNCTION()
	void QuitClicked();
};
