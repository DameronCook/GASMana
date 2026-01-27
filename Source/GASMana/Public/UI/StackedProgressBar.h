// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StackedProgressBar.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class GASMANA_API UStackedProgressBar : public UUserWidget
{
	GENERATED_BODY()

	virtual void NativePreConstruct() override;
	
	UPROPERTY(meta = (BindWidget))
	UImage* StackedBarImage;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY(EditAnywhere)
	UMaterialInstance* MaterialInstance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float AnimSpeed = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float BottomProgress = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess))
	float PreviousTopProgress = 0.0f;

public:
	void UpdateProgressBar(float DeltaTime, float TopProgress);

};
