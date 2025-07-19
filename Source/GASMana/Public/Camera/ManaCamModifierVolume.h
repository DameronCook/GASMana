// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/ManaCameraModifier.h"
#include "ManaCamModifierVolume.generated.h"

class AManaCameraModificationVolume;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GASMANA_API UManaCamModifierVolume : public UManaCameraModifier
{
	GENERATED_BODY()
	
public:
	UManaCamModifierVolume();
	virtual bool ModifyCamera(float DeltaTime, struct FMinimalViewInfo& InOutPOV) override;
};
