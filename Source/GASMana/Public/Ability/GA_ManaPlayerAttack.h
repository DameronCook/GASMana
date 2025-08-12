// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ManaPlayerAttack.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UGA_ManaPlayerAttack : public UGameplayAbility
{
	GENERATED_BODY()

	UGA_ManaPlayerAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UFUNCTION()
	void OnMontageEnded();

	//UFUNCTION()
	//void OnGameplayEventReceived(FGameplayEventData const Payload);

public:
	void EndAbilityAndListenForCombo();
	
};
