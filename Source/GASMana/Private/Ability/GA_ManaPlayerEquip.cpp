// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerEquip.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "../GASManaCharacter.h"
#include "ManaPlayerAnimInstance.h"

UGA_ManaPlayerEquip::UGA_ManaPlayerEquip()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.Action.Equip")));
	SetAssetTags(Tags);

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsEquipping")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsHooked")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsMantling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsZipToPoint")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));
}

void UGA_ManaPlayerEquip::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Couldn't Commit Equip!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	AGASManaCharacter* GASCharacter = Cast<AGASManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();


	if (GASCharacter)
	{
		FName MontageSectionName;
		if (GASCharacter->EquipmentState == EEquipmentState::EES_Unequipped)
		{
			//Play Equip section of montage
			MontageSectionName = "Equip";
		}
		else
		{
			//Play UnEquip section of montage
			MontageSectionName = "Unequip";
			GASCharacter->EquipmentState = EEquipmentState::EES_Unequipped;
		}

		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(GASCharacter->GetEquipEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		}

		// Play the montage and bind delegates
		if (ActorInfo->AvatarActor.IsValid())
		{
			if (GASCharacter->GetEquipRightMontage())
			{
				UAbilityTask_PlayMontageAndWait* MontageRightTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GASCharacter->GetEquipRightMontage(), 1.0f, MontageSectionName, false, 0.0f);

				if (MontageRightTask)
				{
					MontageRightTask->ReadyForActivation();
				}
			}

			if (GASCharacter->GetEquipLeftMontage())
			{
				UAbilityTask_PlayMontageAndWait* MontageLeftTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, GASCharacter->GetEquipLeftMontage(), 1.0f, MontageSectionName, false, 0.0f);

				GASCharacter->GetMesh()->GetAnimInstance()->Montage_Play(GASCharacter->GetEquipLeftMontage());
				if (MontageLeftTask)
				{
					MontageLeftTask->ReadyForActivation();
				}
			}
		}
	}

	// Listen for the gameplay event triggered by the AnimNotify
	AbilitySystemComponent->GenericGameplayEventCallbacks.FindOrAdd(FGameplayTag::RequestGameplayTag(FName("Event.Montage.Equip")))
		.AddUObject(this, &UGA_ManaPlayerEquip::OnMontageSectionEnded);
}

void UGA_ManaPlayerEquip::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString("End Ability Called!"));

	if (AGASManaCharacter* GASCharacter = Cast<AGASManaCharacter>(ActorInfo->AvatarActor.Get()))
	{
		UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();


		//Update the anim instance
		UManaPlayerAnimInstance* AnimInstance = Cast<UManaPlayerAnimInstance>(GASCharacter->GetMesh()->GetAnimInstance());
		if (AnimInstance)
		{
			AnimInstance->SetIsEquipping(false);
			//AnimInstance->Montage_Stop(0.1f, GASCharacter->GetEquipMontage());

		}

		if (GASCharacter && AbilitySystemComponent)
		{
			//AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetFreeEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
			FGameplayTagContainer Tags;
			Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsEquipping")));
			AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Tags);
		}
	}
}

void UGA_ManaPlayerEquip::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString("Cancel Ability Called!"));

}

void UGA_ManaPlayerEquip::OnMontageEnded()
{
	// End the ability (get the current context)
	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, FString("Montage Ended Called!"));
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_ManaPlayerEquip::OnMontageSectionEnded(const FGameplayEventData* Payload)
{
    EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}
