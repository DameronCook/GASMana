// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EnemyCounter.generated.h"

class UTextBlock;
/**
 * 
 */
UCLASS()
class GASMANA_API UEnemyCounter : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, meta=(BindWidget))
	UTextBlock* EnemyCounterText;
public:
	
	UFUNCTION(BlueprintCallable)
	void SetCounterText(FText Text);
};
