// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/CameraTarget.h"
#include "Components/Image.h"


// ReSharper disable once CppMemberFunctionMayBeConst
void UCameraTarget::SetTargetBrush(const FSlateBrush& InBrush)
{
	if (TargetImage)
	{
		TargetImage->SetBrush(InBrush);
	}
}
