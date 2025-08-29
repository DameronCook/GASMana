// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ManaPlayerAirAttack.generated.h"

class APlayerManaCharacter;
/**
 * 
 */
UCLASS()
class GASMANA_API UGA_ManaPlayerAirAttack : public UGameplayAbility
{
	GENERATED_BODY()
	
	UGA_ManaPlayerAirAttack();


	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo
	                             ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
public:
	UFUNCTION()
	void OnJumpLanded();
};
