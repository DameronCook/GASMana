// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerCharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UPlayerCharacterInterface : public UInterface
{
	GENERATED_BODY()
};


class GASMANA_API IPlayerCharacterInterface
{
	GENERATED_BODY()

public:
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
	virtual bool GotMovementInput() const = 0;
	virtual void SetOverlappingItem(class AItem* Item) = 0;
};
