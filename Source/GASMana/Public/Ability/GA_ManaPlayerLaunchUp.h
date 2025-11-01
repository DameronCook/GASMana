// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ManaPlayerLaunchUp.generated.h"

class APlayerManaCharacter;
class UAbilityTask_ApplyRootMotionMoveToActorForce;
/**
 * 
 */
UCLASS()
class GASMANA_API UGA_ManaPlayerLaunchUp : public UGameplayAbility
{
	GENERATED_BODY()

	UGA_ManaPlayerLaunchUp();

	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo
								 ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY()
	UAbilityTask_ApplyRootMotionMoveToActorForce* RootMotionTask;

	UPROPERTY()
	APlayerManaCharacter* PlayerCharacter;

	// You should keep the timer handle in one of your properties
	FTimerHandle EndLaunchTimerHandle;

	
	FActiveGameplayEffectHandle HookBlockEffectHandle;


	UFUNCTION()
	void EndLaunchUp();

	UFUNCTION()
	void ReachedDestination(bool DestinationReached, bool TimedOut, FVector FinalTargetLocation);

public:
	UFUNCTION()
	void CancelMotionTask();	
};
