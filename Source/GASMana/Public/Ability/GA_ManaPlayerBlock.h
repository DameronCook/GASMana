// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "GA_ManaPlayerBlock.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UGA_ManaPlayerBlock : public UGameplayAbility
{
	GENERATED_BODY()

	UGA_ManaPlayerBlock();

	/** Airborne Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> BlockEffectClass;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

};
