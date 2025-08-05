// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability/GA_ManaPlayerHook.h"
#include "PlayerManaCharacter.h"

UGA_ManaPlayerHook::UGA_ManaPlayerHook()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.Hook")));
	SetAssetTags(Tags);

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsHooked")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsMantling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsZipToPoint")));
}

void UGA_ManaPlayerHook::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Couldn't Commit Roll!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();


	if (PlayerCharacter)
	{
		PlayerCharacter->GetHookShot()->AttemptGrapple();
		PlayerCharacter->SetHookAbility(this);
		PlayerCharacter->InstantlyUnequipGear();
		/*
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, FString("Attempting Grapple!"));
		}
		*/
		if (AbilitySystemComponent)
		{
			AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetHookEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		}
	}
}

void UGA_ManaPlayerHook::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get()))
	{
		PlayerCharacter->SetHookAbility(nullptr);
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, FString("End Ability Called!"));

		UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

		if (PlayerCharacter && AbilitySystemComponent)
		{
			//AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetFreeEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
			FGameplayTagContainer Tags;
			Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsHooked")));
			AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Tags);

		}
	}

}

void UGA_ManaPlayerHook::EndHook()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}