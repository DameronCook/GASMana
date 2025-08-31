// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GA_ManaPlayerFocus.generated.h"

class APlayerManaCharacter;
/**
 * 
 */
UCLASS()
class GASMANA_API UGA_ManaPlayerFocus : public UGameplayAbility
{
	GENERATED_BODY()

	UGA_ManaPlayerFocus();
	
	TArray<class AActor*> FindAllActorsInRange(const APlayerManaCharacter* PlayerCharacter, float Radius) const;
	bool DrawLineToTarget(const ACharacter* Character, const AActor* OverlappedActor) const;
	static float CalculateAngleToTarget(ACharacter* Character, const AActor* OverlappedActor);

	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo
								 ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

public:
	
	void EndFocusAbility();
};
