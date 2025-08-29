// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerMantle.h"
#include "../../Public/PlayerManaCharacter.h"


UGA_ManaPlayerMantle::UGA_ManaPlayerMantle()
{

	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.Mantle")));
	SetAssetTags(Tags);

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallJumping")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsMantling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsZipToPoint")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsHooked")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsEquipping")));
}

void UGA_ManaPlayerMantle::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo
                                           ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	if (PlayerCharacter)
	{
		PlayerCharacter->SetMantleAbility(this);
		PlayerCharacter->InstantlyUnequipGear();

		if (PlayerCharacter->GetZipAbility())
		{
			PlayerCharacter->GetHookShot()->EndGrapple();
		}

		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetMantleEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());

		}
	}
}


void UGA_ManaPlayerMantle::OnMantleEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
}