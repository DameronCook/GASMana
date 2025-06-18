// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "I_PickUpInterface.generated.h"

class AActor;
class UAbilitySystemComponent;

UINTERFACE(MinimalAPI)
class UI_PickUpInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GASMANA_API II_PickUpInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Pickup")
	void OnPickedUp(AActor* Picker, UAbilitySystemComponent* AbilitySystem);
};
