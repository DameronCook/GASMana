// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/ManaCameraModifier.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

#include "Camera/ManaSpringArmComponent.h"

bool UManaCameraModifier::ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
	Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

	if (!IsValid(ViewTarget))
	{
		return false;
	}

	APawn* Pawn = Cast<APawn>(ViewTarget);

	if (!IsValid(Pawn))
	{
		return false;
	}

	APlayerController* PlayerController = Cast<APlayerController>(Pawn->Controller);

	if (!IsValid(PlayerController))
	{
		return false;
	}

	if (!PlayerController->RotationInput.IsNearlyZero(THRESH_QUAT_NORMALIZED))
	{
		CooldownRemaining = CooldownAfterPlayerInput;
		return false;
	}

	if (CooldownRemaining > 0)
	{
		CooldownRemaining -= DeltaTime;
		return false;
	}

	return false;
}

FManaCameraInfo UManaCameraModifier::GetCurrentModifiers()
{
	return CurrentModifiers;
}

FManaCameraInfo UManaCameraModifier::GetTargetModifiers()
{
	return TargetModifiers;
}

void UManaCameraModifier::ApplyCameraTransition(const FManaCameraInfo& CamInfo, const float TransitionTime, FMinimalViewInfo& MinimalViewInfo, const float DeltaTime)
{
    if (!TargetModifiers.Equals(CamInfo))
    {
        CurrentModifiers = TargetModifiers;
        TargetModifiers = CamInfo;

        if (RemainingTransitionTime > 0.f)
        {
            RemainingTransitionTime = TotalTransitionTime - RemainingTransitionTime;
        }
    }

    if (CurrentModifiers.Equals(TargetModifiers))
    {
        ApplyCameraInfo(CurrentModifiers, 1.0f, MinimalViewInfo);
        return;
    }

    float ApplicationFactor = 0.f;
    FManaCameraInfo ModifiersToApply;

    if (!TargetModifiers.IsZero())
    {
        //Apply new modifiers
        ModifiersToApply = TargetModifiers;

        if (RemainingTransitionTime <= 0.0f)
        {
            TotalTransitionTime = TransitionTime;
            RemainingTransitionTime = TransitionTime;
        }
        else
        {
            RemainingTransitionTime -= DeltaTime;

            if (RemainingTransitionTime <= 0.0f)
            {
                //Transition finished
                CurrentModifiers = TargetModifiers;
                ApplicationFactor = 1.0f;
            }
            else
            {
                ApplicationFactor = 1 - (RemainingTransitionTime / TotalTransitionTime);
            }

        }
    }
    else
    {
        //Stop applying Modifiers

        ModifiersToApply = CurrentModifiers;

        if (RemainingTransitionTime <= 0.0f)
        {
            //Restart the timer
            RemainingTransitionTime = TotalTransitionTime;
            ApplicationFactor = 1.0f;
        }
        else
        {
            RemainingTransitionTime -= DeltaTime;

            if (RemainingTransitionTime <= 0.0f)
            {
                CurrentModifiers = TargetModifiers;
                ApplicationFactor = 0.0f;
            }
            else
            {
                ApplicationFactor = RemainingTransitionTime / TotalTransitionTime;
            }
        }
    }

    ApplyCameraInfo(ModifiersToApply, ApplicationFactor, MinimalViewInfo);
}

bool UManaCameraModifier::PlayerHasRecentlyChangedCamera() const
{
	return CooldownRemaining > 0;
}

void UManaCameraModifier::ApplyCameraInfo(const FManaCameraInfo& CamInfo, const float Factor, FMinimalViewInfo& MinimalViewInfo) const
{
    AActor* viewTarget = GetViewTarget();

    if (!IsValid(viewTarget))
    {
        return;
    }

    TArray<USceneComponent*> children;
    viewTarget->GetRootComponent()->GetChildrenComponents(true, children);

    // Apply FOV.
    float appliedFOV = CamInfo.FOV * Factor;
    MinimalViewInfo.FOV = MinimalViewInfo.FOV + appliedFOV;

    // Apply spring arm length.
    for (int index = 0; index < children.Num(); ++index)
    {
        UManaSpringArmComponent* springArmComponent = Cast<UManaSpringArmComponent>(children[index]);

        if (IsValid(springArmComponent))
        {
            springArmComponent->TargetArmLengthModifier += CamInfo.SpringArmLength * Factor;
            break;
        }
    }

    // Apply depth of field.
    if (!FMath::IsNearlyZero(CamInfo.DepthOfFieldFocalDistance))
    {
        MinimalViewInfo.PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
        MinimalViewInfo.PostProcessSettings.DepthOfFieldFocalDistance += CamInfo.DepthOfFieldFocalDistance * Factor;
    }

    if (!FMath::IsNearlyZero(CamInfo.DepthOfFieldFocalRegion))
    {
        MinimalViewInfo.PostProcessSettings.bOverride_DepthOfFieldFocalRegion = true;
        MinimalViewInfo.PostProcessSettings.DepthOfFieldFocalRegion += CamInfo.DepthOfFieldFocalRegion * Factor;
    }

    if (!FMath::IsNearlyZero(CamInfo.DepthOfFieldNearTransitionRegion))
    {
        MinimalViewInfo.PostProcessSettings.bOverride_DepthOfFieldNearTransitionRegion = true;
        MinimalViewInfo.PostProcessSettings.DepthOfFieldNearTransitionRegion += CamInfo.DepthOfFieldNearTransitionRegion * Factor;
    }

    if (!FMath::IsNearlyZero(CamInfo.DepthOfFieldFarTransitionRegion))
    {
        MinimalViewInfo.PostProcessSettings.bOverride_DepthOfFieldFarTransitionRegion = true;
        MinimalViewInfo.PostProcessSettings.DepthOfFieldFarTransitionRegion += CamInfo.DepthOfFieldFarTransitionRegion * Factor;
    }

    if (!FMath::IsNearlyZero(CamInfo.DepthOfFieldNearBlurSize))
    {
        MinimalViewInfo.PostProcessSettings.bOverride_DepthOfFieldNearBlurSize = true;
        MinimalViewInfo.PostProcessSettings.DepthOfFieldNearBlurSize += CamInfo.DepthOfFieldNearBlurSize * Factor;
    }

    if (!FMath::IsNearlyZero(CamInfo.DepthOfFieldFarBlurSize))
    {
        MinimalViewInfo.PostProcessSettings.bOverride_DepthOfFieldFarBlurSize = true;
        MinimalViewInfo.PostProcessSettings.DepthOfFieldFarBlurSize += CamInfo.DepthOfFieldFarBlurSize * Factor;
    }
}
