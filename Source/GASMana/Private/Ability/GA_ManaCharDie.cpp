// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaCharDie.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "GASMana/GASManaCharacter.h"


UGA_ManaCharDie::UGA_ManaCharDie()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.Action.Die")));
	SetAssetTags(Tags);

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsDying")));

}

void UGA_ManaCharDie::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	//Next, Commit the ability
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Couldn't Commit DEATH!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	const AGASManaCharacter* Character = Cast<AGASManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	
	if (Character) {
		Character->PlayFlashEffect(Character->HitFlashColor, 0.2f);
		Character->RemoveFreeTag();
		// Play the montage and bind delegates
		if (Character->GetDeathMontage() && ActorInfo->AvatarActor.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(Character->GetDeathEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());

			if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Character->GetDeathMontage(), 1.0f, Character->GetDeathMontageSection()))
			{
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, Character->GetHitReactMontage()->GetName());
				MontageTask->OnCompleted.AddDynamic(this, &UGA_ManaCharDie::OnMontageEnded);
				MontageTask->OnInterrupted.AddDynamic(this, &UGA_ManaCharDie::OnMontageEnded);
				MontageTask->OnCancelled.AddDynamic(this, &UGA_ManaCharDie::OnMontageEnded);
				MontageTask->OnBlendOut.AddDynamic(this, &UGA_ManaCharDie::OnMontageEnded);
				MontageTask->ReadyForActivation();
			}
		}
		else
		{
			// If no montage, just end ability immediately
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
	}
}

void UGA_ManaCharDie::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGA_ManaCharDie::OnMontageEnded()
{
	// End the ability (get the current context)
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
