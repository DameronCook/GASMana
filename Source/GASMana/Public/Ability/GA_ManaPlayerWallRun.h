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

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
