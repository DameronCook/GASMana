// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayEffect.h"
#include "PlayerManaCharacter.h"
#include "GA_ManaPlayerBlock.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UGA_ManaPlayerBlock : public UGameplayAbility
{
	GENERATED_BODY()

	UGA_ManaPlayerBlock();
	TArray<class AActor*> FindAllActorsInRange(const APlayerManaCharacter* PlayerCharacter, float Radius) const;
	bool DrawLineToTarget(const ACharacter* Character, const AActor* OverlappedActor) const;
	static float CalculateAngleToTarget(ACharacter* Character, const AActor* OverlappedActor);

	/** Block Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Block", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> BlockEffectClass;

	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo
	                             ActivationInfo, const FGameplayEventData* TriggerEventData) override;
};
