// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerWallRun.h"
#include "../../Public/PlayerManaCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"

UGA_ManaPlayerWallRun::UGA_ManaPlayerWallRun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.WallRun")));
	SetAssetTags(Tags);

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsGrounded")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));
}

void UGA_ManaPlayerWallRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	if (PlayerCharacter && AbilitySystemComponent)
	{
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetWallRunEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetBlockMovementEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());

		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PlayerCharacter->GetWallRunMontage(), 1.0f, NAME_None, false, 0.0f);
	}
}