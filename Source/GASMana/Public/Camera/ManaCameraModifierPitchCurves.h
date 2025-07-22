// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/ManaCameraModifier.h"
#include "ManaCameraModifierPitchCurves.generated.h"

/**
 * Applies different curves effects depending on the pitch of the camera
 */
UCLASS(BlueprintType, Blueprintable)
class GASMANA_API UManaCameraModifierPitchCurves : public UManaCameraModifier
{
	GENERATED_BODY()
	
	UManaCameraModifierPitchCurves();

public:

	virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;


protected:
	//Curve to convert changes in pitch to changes in distance
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Custom, meta = (AllowAbstract = "true"))
	UCurveFloat* PitchToDistanceCurve;

	//Curve to convert changes in pitch to changes in FOV
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Custom, meta = (AllowAbstract = "true"))
	UCurveFloat* PitchToFOVCurve;
};
