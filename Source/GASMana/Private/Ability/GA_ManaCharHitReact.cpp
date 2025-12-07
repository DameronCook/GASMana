// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaCharHitReact.h"

#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
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

	/* We must *not* block anything except rolling since that's how the player enters this state
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsBlocking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsHooked")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsEquipping")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsFree")));
	*/
}

void UGA_ManaCharHitReact::ActivateAbility(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Activating Hit React Ability!"));

	//Next, Commit the ability
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Couldn't Commit HitReact!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	AGASManaCharacter* Character = Cast<AGASManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	
	if (Character) {
		// Play the montage and bind delegates
		if (Character->GetHitReactMontage() && ActorInfo->AvatarActor.IsValid())
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Next Combo Section: %s"), *PlayerCharacter->GetNextAttackMontageSection().ToString()));

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

		AbilitySystemComponent->ApplyGameplayEffectToSelf(Character->GetHitReactEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		
	}
}

void UGA_ManaCharHitReact::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_ManaCharHitReact::OnMontageEnded()
{
	// End the ability (get the current context)
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}