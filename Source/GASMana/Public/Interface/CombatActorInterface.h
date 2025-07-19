// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatActorInterface.generated.h"

/* Interface for actors that may participate in combat */
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UCombatActorInterface : public UInterface
{
	GENERATED_BODY()
};

class GASMANA_API ICombatActorInterface
{
	GENERATED_BODY()

	virtual bool IsInCombat() = 0;
};
