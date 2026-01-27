// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/StackedProgressBar.h"
#include "Components/Image.h"

void UStackedProgressBar::NativePreConstruct()
{
	Super::NativePreConstruct();
	DynamicMaterial = UMaterialInstanceDynamic::Create(MaterialInstance, this);
	if (StackedBarImage && DynamicMaterial)
	{
		StackedBarImage->SetBrushFromMaterial(DynamicMaterial);
	}
}

void UStackedProgressBar::UpdateProgressBar(float DeltaTime, float TopProgress)
{
	if (DynamicMaterial)
	{
		const float NewBottomProgress = (TopProgress - PreviousTopProgress) * -1;
		BottomProgress = NewBottomProgress + BottomProgress;

		PreviousTopProgress = TopProgress;


		DynamicMaterial->SetScalarParameterValue("Top Progress", TopProgress);
		DynamicMaterial->SetScalarParameterValue("Bottom Progress", BottomProgress);
		
		BottomProgress = FMath::Clamp(BottomProgress - (AnimSpeed * DeltaTime), 0.f, 1.f);


		
	}
}
