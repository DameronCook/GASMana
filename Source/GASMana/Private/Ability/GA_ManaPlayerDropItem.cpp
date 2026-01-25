// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerDropItem.h"

#include "PlayerManaCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_ManaPlayerDropItem::UGA_ManaPlayerDropItem()
{

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.Drop")));
	SetAssetTags(Tags);

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsDropping")));

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsMantling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsBlocking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsSwinging")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsZipToPoint")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsHooked")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsEquipping")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsHitReacting")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsShieldStun")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsDying")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsDropping")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));
	
}

void UGA_ManaPlayerDropItem::ActivateAbility(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APlayerManaCharacter* Character = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());

	if (Character)
	{
		Character->RemoveFreeTag();
		if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Character->GetCurrentDropMontage(), 1.0f, Character->GetNextAttackMontageSection(), false, 0.0f))
		{
			MontageTask->OnCompleted.AddDynamic(this, &UGA_ManaPlayerDropItem::End);
			MontageTask->OnInterrupted.AddDynamic(this, &UGA_ManaPlayerDropItem::End);
			MontageTask->OnCancelled.AddDynamic(this, &UGA_ManaPlayerDropItem::End);
			MontageTask->OnBlendOut.AddDynamic(this, &UGA_ManaPlayerDropItem::End);
			MontageTask->ReadyForActivation();
		}
	}
	else
	{
		// If no montage, just end ability immediately
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

void UGA_ManaPlayerDropItem::End()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}

void UGA_ManaPlayerDropItem::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	APlayerManaCharacter* Character = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());

	if (Character && Character->GetAbilitySystemComponent())
	{
		Character->GetAbilitySystemComponent()->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.IsFree")));
	}
}
