// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CameraTarget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class GASMANA_API UCameraTarget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> TargetImage;

public:
	void SetTargetBrush(const FSlateBrush& InBrush);
};
