// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TargetingActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UTargetingActorInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GASMANA_API ITargetingActorInterface
{
	GENERATED_BODY()

	virtual bool IsSelectingTarget() const = 0;
	virtual AActor* GetCurrentTarget() const = 0;
	virtual FVector2D GetCurrentTargetSelectionInput() const = 0;
};
