// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ManaPlayerZipToPoint.generated.h"

class  UAbilityTask_ApplyRootMotionMoveToActorForce;
/**
 * 
 */
UCLASS()
class GASMANA_API UGA_ManaPlayerZipToPoint : public UGameplayAbility
{
	GENERATED_BODY()

	UGA_ManaPlayerZipToPoint();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UAbilityTask_ApplyRootMotionMoveToActorForce* RootMotionTask;

	UFUNCTION()
	void ReachedDestination(bool DestinationReached, bool TimedOut, FVector FinalTargetLocation);

public:
	UFUNCTION()
	void CancelMotionTask();
};
