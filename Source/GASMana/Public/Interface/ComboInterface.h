// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ComboInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UComboInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GASMANA_API IComboInterface
{
	GENERATED_IINTERFACE_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetDefaultCombos() = 0;
	virtual void SetNextComboSegment(FName NextCombo) = 0;
};
