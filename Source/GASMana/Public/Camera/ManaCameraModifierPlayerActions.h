// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/ManaCameraModifier.h"
#include "ManaCameraModifierPlayerActions.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UManaCameraModifierPlayerActions : public UManaCameraModifier
{
	GENERATED_BODY()
	
public:

	UManaCameraModifierPlayerActions();

	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;
	virtual bool ProcessViewRotation(class AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot) override;


private: 
	
	/*Rate at which the camera should follow the turning player, in degrees per second*/
	UPROPERTY(EditDefaultsOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	float RotationSpeed;

	FRotator GetDeltaCameraRotation(float DeltaTime, float InterpSpeed, FRotator CurrentRotation, FRotator FinalRotation);

	FVector GetDeltaCameraLotation(float DeltaTime, float InterpSpeed, FVector CurrentLocation, FVector FinalLocation);
};
