// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ManaCharHitReact.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UGA_ManaCharHitReact : public UGameplayAbility
{
	GENERATED_BODY()

	UGA_ManaCharHitReact();

	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo
								 ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	UFUNCTION()
	void OnMontageEnded();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cam", meta = (AllowPrivateAccess = true))
	TSubclassOf<UCameraShakeBase> HitReactCameraShake;
};
