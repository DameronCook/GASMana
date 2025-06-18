// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_ProgressBarInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UI_ProgressBarInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GASMANA_API II_ProgressBarInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.


public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ProgressBar")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ProgressBar")
	float GetHealthAsRatio() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ProgressBar")
	float GetStamina() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ProgressBar")
	float GetStaminaAsRatio() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ProgressBar")
	float GetMana() const;
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "ProgressBar")
	float GetManaAsRatio() const;
};
