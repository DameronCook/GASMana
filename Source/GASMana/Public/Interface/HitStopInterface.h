// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitStopInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitStopInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class GASMANA_API IHitStopInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Sets default values for this component's properties
	FTimerHandle HitStopTimerHandle;

	FTimerManager TimerManager;

	void StartHitStop(float Duration, AActor* HitStopActor);

	UFUNCTION()
	virtual void EndHitStop(AActor* HitStopActor);
};
