#pragma once

#include "GameFramework/RootMotionSource.h"

struct FRootMotionSource_DynamicForce : public FRootMotionSource_ConstantForce
{
    float Strength = 1.0f;

    // Expose a setter for direction
    void SetForceDirection(const FVector& NewDirection)
    {
        Force = NewDirection * Strength;
    }

    // Optionally override Update to use the latest direction
    virtual FRootMotionSource* Clone() const override
    {
        return new FRootMotionSource_DynamicForce(*this);
    }

    virtual bool UpdateStateFrom(const FRootMotionSource* SourceToTakeStateFrom, bool bMarkForSimulatedCatchup = false) override
    {
        return FRootMotionSource_ConstantForce::UpdateStateFrom(SourceToTakeStateFrom, bMarkForSimulatedCatchup);
    }
    /*
    virtual void PrepareRootMotion(
        float SimulationTime,
        float MovementTickTime,
        const ACharacter& Character,
        const UCharacterMovementComponent& MoveComponent) override
    {
        FVector CurrentForce = Force; // Force should be updated by SetForceDirection
        if (!CurrentForce.IsNearlyZero())
        {
            // Set the translation for this tick's root motion
            RootMotionTransform.SetTranslation(CurrentForce * MovementTickTime);
        }
        else
        {
            RootMotionTransform.SetTranslation(FVector::ZeroVector);
        }
    }
    */
};

