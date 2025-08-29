// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ManaPlayerWallRun.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UGA_ManaPlayerWallRun : public UGameplayAbility
{
	GENERATED_BODY()
	
	UGA_ManaPlayerWallRun();

	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo
	                             ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	FActiveGameplayEffectHandle ManaDrainEffectHandle;
	FActiveGameplayEffectHandle WallRunEffectHandle;


public:
	UFUNCTION()
	void OnWallRunFinished();
	
	FORCEINLINE FActiveGameplayEffectHandle GetWallRunEffectHandle() { return WallRunEffectHandle; }

};
