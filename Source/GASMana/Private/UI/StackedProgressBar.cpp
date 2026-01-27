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
		GEngine->AddOnScreenDebugMessage(3, 1.f, FColor::Green, FString::Printf(TEXT("New Bottom Progress:  %f"), BottomProgress));

		PreviousTopProgress = TopProgress;

		GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Green, FString::Printf(TEXT("Top Progress: %f"), TopProgress));
		GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Green, FString::Printf(TEXT("Bottom Progress: %f"), BottomProgress));

		DynamicMaterial->SetScalarParameterValue("Top Progress", TopProgress);
		DynamicMaterial->SetScalarParameterValue("Bottom Progress", BottomProgress);
		
		BottomProgress = FMath::Clamp(BottomProgress - (AnimSpeed * DeltaTime), 0.f, 1.f);


		
	}
}
