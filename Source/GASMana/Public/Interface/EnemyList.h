// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyList.generated.h"

class ABaseManaEnemy;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UEnemyList : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GASMANA_API IEnemyList
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void RemoveEnemyFromList(ABaseManaEnemy* Enemy);
};
