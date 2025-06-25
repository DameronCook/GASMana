// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerBlock.h"
#include "../../Public/PlayerManaCharacter.h"

UGA_ManaPlayerBlock::UGA_ManaPlayerBlock()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.Block")));
	SetAssetTags(Tags);

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsBlocking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
}



void UGA_ManaPlayerBlock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//Next, Commit the ability
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	if (PlayerCharacter && PlayerCharacter->GetBlockingEffectClass() && AbilitySystemComponent)
	{	
		AbilitySystemComponent->ApplyGameplayEffectToSelf(
			PlayerCharacter->GetBlockingEffectClass()->GetDefaultObject<UGameplayEffect>(),
			1.0f,
			AbilitySystemComponent->MakeEffectContext()
		);

		PlayerCharacter->Blocking();
	}

	//Update Stamina Regen
	PlayerCharacter->UpdateStaminaRegen();

	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
