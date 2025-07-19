// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "ManaCameraInfo.h"
#include "ManaCameraModificationVolume.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API AManaCameraModificationVolume : public ATriggerVolume
{
	GENERATED_BODY()
	
public:
	AManaCameraModificationVolume();

	/*Sets the currently applied modification volume for the specified actor*/
	virtual void NotifyActorBeginOverlap(AActor* Other) override;

	/*Resets the previously applied modification volume for the specified actor*/
	virtual void NotifyActorEndOverlap(AActor* Other) override;

	/* Time in seconds it takes to fully apply the transition*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraModifiers)
	float TransitionTime;

	/* Value to ADD to the current properties of the camera modifier*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = CameraModifiers)
	FManaCameraInfo Modifiers;
};
