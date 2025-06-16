// Fill out your copyright notice in the Description page of Project Settings.

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilityTasks/ApplyRootMotionDynamicForce.h"
#include "AbilityTasks/RootMotionSource_DynamicForce.h"

UApplyRootMotionDynamicForce* UApplyRootMotionDynamicForce::ApplyRootMotionDynamicForce(
    UGameplayAbility* OwningAbility,
    FName TaskInstanceName,
    FVector DynamicWorldDirection,
    float DynamicStrength,
    float DynamicDuration,
    bool DynamicbIsAdditive,
    UCurveFloat* DynamicStrengthOverTime,
    ERootMotionFinishVelocityMode DynamicVelocityOnFinishMode,
    FVector DynamicSetVelocityOnFinish,
    float DynamicClampVelocityOnFinish,
    bool bDynamicEnableGravity)
{
    UApplyRootMotionDynamicForce* MyTask = NewAbilityTask<UApplyRootMotionDynamicForce>(OwningAbility, TaskInstanceName);

    if (MyTask && OwningAbility)
    {
        MyTask->CurrentDirection = DynamicWorldDirection;
        MyTask->Duration = DynamicDuration;
        

        if (ACharacter* Character = Cast<ACharacter>(OwningAbility->GetAvatarActorFromActorInfo()))
        {
            if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
            {
                TSharedPtr<FRootMotionSource_DynamicForce> DynamicSource = MakeShared<FRootMotionSource_DynamicForce>();
                DynamicSource->InstanceName = TaskInstanceName;
                DynamicSource->AccumulateMode = DynamicbIsAdditive ? ERootMotionAccumulateMode::Additive : ERootMotionAccumulateMode::Override;
                DynamicSource->Priority = 5; // Set as needed
                DynamicSource->Force = DynamicWorldDirection * DynamicStrength;
                DynamicSource->Strength = DynamicStrength;
                DynamicSource->Duration = DynamicDuration;
                // Set other properties as needed

                MyTask->DynamicRootMotionSourceID = MoveComp->ApplyRootMotionSource(DynamicSource);
            }
        }
        MyTask->SharedInitAndApply();
    }
    return MyTask;
}

void UApplyRootMotionDynamicForce::SetForceDirection(FVector NewDirection)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 0.01f, FColor::Cyan, FString::Printf(TEXT("SetForceDirection: %s"), *NewDirection.ToString()));
    }

    if (ACharacter* Character = Cast<ACharacter>(GetAvatarActor()))
    {
        if (UCharacterMovementComponent* MoveComp = Character->GetCharacterMovement())
        {
            FRootMotionSourceGroup& Group = MoveComp->CurrentRootMotion;
            TSharedPtr<FRootMotionSource> Source = Group.GetRootMotionSourceByID(DynamicRootMotionSourceID);
            if (Source.IsValid())
            {
                FRootMotionSource_DynamicForce* DynamicSource = static_cast<FRootMotionSource_DynamicForce*>(Source.Get());
                if (DynamicSource)
                {
                    DynamicSource->SetForceDirection(NewDirection);
                }
            }
        }
    }
}

void UApplyRootMotionDynamicForce::TickTask(float DeltaTime)
{
    SetForceDirection(CurrentDirection);

    Super::TickTask(DeltaTime);

    //ElapsedTime += DeltaTime;
    //if (ElapsedTime >= Duration)
    //{
    //    if (!bIsFinished)
    //    {
    //        bIsFinished = true;
    //        OnFinish.Broadcast();
    //        EndTask();
    //    }
    //}

}
