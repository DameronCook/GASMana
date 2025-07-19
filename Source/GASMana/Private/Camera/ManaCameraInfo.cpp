// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaCameraInfo.h"

bool FManaCameraInfo::Equals(const FManaCameraInfo& Other) const
{
    FManaCameraInfo delta;
    delta.FOV = FOV - Other.FOV;
    delta.SpringArmLength = SpringArmLength - Other.SpringArmLength;
    delta.DepthOfFieldFocalDistance = DepthOfFieldFocalDistance - Other.DepthOfFieldFocalDistance;
    delta.DepthOfFieldFocalRegion = DepthOfFieldFocalRegion - Other.DepthOfFieldFocalRegion;
    delta.DepthOfFieldNearTransitionRegion = DepthOfFieldNearTransitionRegion - Other.DepthOfFieldNearTransitionRegion;
    delta.DepthOfFieldFarTransitionRegion = DepthOfFieldFarTransitionRegion - Other.DepthOfFieldFarTransitionRegion;
    delta.DepthOfFieldNearBlurSize = DepthOfFieldNearBlurSize - Other.DepthOfFieldNearBlurSize;
    delta.DepthOfFieldFarBlurSize = DepthOfFieldFarBlurSize - Other.DepthOfFieldFarBlurSize;
    return delta.IsZero();
}

bool FManaCameraInfo::IsZero() const
{
    return FMath::Abs(FOV) < SMALL_NUMBER && FMath::Abs(SpringArmLength) < SMALL_NUMBER &&
        FMath::Abs(DepthOfFieldFocalDistance) < SMALL_NUMBER && FMath::Abs(DepthOfFieldFocalRegion) < SMALL_NUMBER &&
        FMath::Abs(DepthOfFieldNearTransitionRegion) < SMALL_NUMBER &&
        FMath::Abs(DepthOfFieldFarTransitionRegion) < SMALL_NUMBER &&
        FMath::Abs(DepthOfFieldNearBlurSize) < SMALL_NUMBER && FMath::Abs(DepthOfFieldFarBlurSize) < SMALL_NUMBER;
}