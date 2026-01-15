// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaCharHitReact.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actors/BaseManaEnemy.h"
#include "GASMana/GASManaCharacter.h"

UGA_ManaCharHitReact::UGA_ManaCharHitReact()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.Action.HitReact")));
	SetAssetTags(Tags);

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsHitReacting")));

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
}

void UGA_ManaCharHitReact::ActivateAbility(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	//Next, Commit the ability
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Couldn't Commit HitReact!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (const AGASManaCharacter* Character = Cast<AGASManaCharacter>(ActorInfo->AvatarActor.Get())) {
		Character->PlayFlashEffect(Character->HitFlashColor, 0.2f);
		// Play the montage and bind delegates
		if (Character->GetHitReactMontage() && ActorInfo->AvatarActor.IsValid())
		{
			if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Character->GetHitReactMontage(), 1.0f, Character->GetHitReactSection()))
			{
				MontageTask->OnCompleted.AddDynamic(this, &UGA_ManaCharHitReact::OnMontageEnded);
				MontageTask->OnInterrupted.AddDynamic(this, &UGA_ManaCharHitReact::OnMontageEnded);
				MontageTask->OnCancelled.AddDynamic(this, &UGA_ManaCharHitReact::OnMontageEnded);
				MontageTask->OnBlendOut.AddDynamic(this, &UGA_ManaCharHitReact::OnMontageEnded);
				MontageTask->ReadyForActivation();
			}
		}
		else
		{
			// If no montage, just end ability immediately
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		if (Character->GetAbilitySystemComponent())
		{
			Character->GetAbilitySystemComponent()->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag("Player.IsAirAttacking"));
		}

	}
}

void UGA_ManaCharHitReact::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	if (const AGASManaCharacter* Character = Cast<ABaseManaEnemy>(ActorInfo->AvatarActor.Get()))
	{
		if (Character->GetLeftHandEquipment())
		{
			if (Character->GetAbilitySystemComponent())
			{
				FGameplayTag BlockTag = FGameplayTag::RequestGameplayTag(FName("Character.IsBlocking"));
				if (!Character->GetAbilitySystemComponent()->HasMatchingGameplayTag(BlockTag))
				{
					Character->GetAbilitySystemComponent()->AddLooseGameplayTag(BlockTag);
				}
			}	
		}
	}
}

void UGA_ManaCharHitReact::OnMontageEnded()
{
	// End the ability (get the current context)
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}