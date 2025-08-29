// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ManaPlayerMantle.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UGA_ManaPlayerMantle : public UGameplayAbility
{
	GENERATED_BODY()
	
	UGA_ManaPlayerMantle();

	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo
	                             ActivationInfo, const FGameplayEventData* TriggerEventData) override;


public:

	void OnMantleEnded();
};
