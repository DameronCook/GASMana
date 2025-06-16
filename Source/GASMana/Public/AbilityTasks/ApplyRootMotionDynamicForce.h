// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/RootMotionSource.h"
#include "ApplyRootMotionDynamicForce.generated.h"

UCLASS()
class GASMANA_API UApplyRootMotionDynamicForce : public UAbilityTask_ApplyRootMotionConstantForce
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName="ApplyRootMotionDynamicForce", HidePin="OwningAbility", DefaultToSelf="OwningAbility"))
    static UApplyRootMotionDynamicForce* ApplyRootMotionDynamicForce( UGameplayAbility* OwningAbility, FName TaskInstanceName, FVector DynamicWorldDirection, float DynamicStrength, float DynamicDuration, bool DynamicbIsAdditive = false, UCurveFloat* DynamicStrengthOverTime = nullptr, ERootMotionFinishVelocityMode DynamicVelocityOnFinishMode = ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity, FVector DynamicSetVelocityOnFinish = FVector::ZeroVector, float DynamicClampVelocityOnFinish = 0.f, bool bDynamicEnableGravity = false);

    // Call this to update the direction at runtime
    UFUNCTION(BlueprintCallable, Category = "Ability|Tasks")
    void SetForceDirection(FVector NewDirection);

protected:
    virtual void TickTask(float DeltaTime) override;

    // Store the latest direction
    FVector CurrentDirection;

    // Store the root motion source ID for updating
    uint16 DynamicRootMotionSourceID;

    //float ElapsedTime = 0.0f;
    //float Duration = 0.0f; 
    //bool bIsFinished = false;
};
