// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaCharacterHitShieldStun.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GASMana/GASManaCharacter.h"

UGA_ManaCharacterHitShieldStun::UGA_ManaCharacterHitShieldStun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.Action.ShieldStun")));
	SetAssetTags(Tags);

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.ShieldStun")));

}

void UGA_ManaCharacterHitShieldStun::ActivateAbility(FGameplayAbilitySpecHandle Handle,
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
		// Play the montage and bind delegates
		if (Character->GetShieldStunMontage() && ActorInfo->AvatarActor.IsValid())
		{
			if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Character->GetShieldStunMontage()))
			{
				MontageTask->OnCompleted.AddDynamic(this, &UGA_ManaCharacterHitShieldStun::OnMontageEnded);
				MontageTask->OnInterrupted.AddDynamic(this, &UGA_ManaCharacterHitShieldStun::OnMontageEnded);
				MontageTask->OnCancelled.AddDynamic(this, &UGA_ManaCharacterHitShieldStun::OnMontageEnded);
				MontageTask->OnBlendOut.AddDynamic(this, &UGA_ManaCharacterHitShieldStun::OnMontageEnded);
				MontageTask->ReadyForActivation();
			}
		}
		else
		{
			// If no montage, just end ability immediately
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		if (UAbilitySystemComponent* ASC = Cast<UAbilitySystemComponent>(Character->GetAbilitySystemComponent()))
		{
			ASC->ApplyGameplayEffectToSelf(Character->GetShieldStunEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, ASC->MakeEffectContext());
		}
	}
}

void UGA_ManaCharacterHitShieldStun::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	
	if (const AGASManaCharacter* Character = Cast<AGASManaCharacter>(ActorInfo->AvatarActor.Get()))
	{
		if (UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent())
		{
			FGameplayTag FreeTag = FGameplayTag::RequestGameplayTag(FName("Character.IsFree"));
			FGameplayTag StunTag = FGameplayTag::RequestGameplayTag(FName("Character.IsShieldStun"));
			ASC->AddLooseGameplayTag(FreeTag);
			ASC->RemoveLooseGameplayTag(StunTag);
		}
	}
}

void UGA_ManaCharacterHitShieldStun::OnMontageEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
