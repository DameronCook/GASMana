// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerSword_Attack_01.h"
#include "../../Public/PlayerManaCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ManaPlayerAnimInstance.h"
#include "../../GASManaCharacter.h"

UGA_ManaPlayerSword_Attack_01::UGA_ManaPlayerSword_Attack_01()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.Action.Attack")));
	SetAssetTags(Tags);

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));
}

void UGA_ManaPlayerSword_Attack_01::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//Next, Commit the ability
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Couldn't Commit Attack!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	
	if (PlayerCharacter) {
		// Play the montage and bind delegates
		if (PlayerCharacter->GetAttackMontage() && ActorInfo->AvatarActor.IsValid())
		{
			UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PlayerCharacter->GetAttackMontage(), 1.0f, NAME_None, false, 0.0f);

			if (MontageTask)
			{
				MontageTask->OnCompleted.AddDynamic(this, &UGA_ManaPlayerSword_Attack_01::OnMontageEnded);
				MontageTask->OnInterrupted.AddDynamic(this, &UGA_ManaPlayerSword_Attack_01::OnMontageEnded);
				MontageTask->OnCancelled.AddDynamic(this, &UGA_ManaPlayerSword_Attack_01::OnMontageEnded);
				MontageTask->OnBlendOut.AddDynamic(this, &UGA_ManaPlayerSword_Attack_01::OnMontageEnded);
				MontageTask->ReadyForActivation();
			}
		}
		else
		{
			// If no montage, just end ability immediately
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Character.Damaged"));

		UAbilityTask_WaitGameplayEvent* WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag, nullptr, true, true);

		if (WaitTask)
		{
			WaitTask->EventReceived.AddDynamic(this, &UGA_ManaPlayerSword_Attack_01::OnGameplayEventReceived);
			WaitTask->ReadyForActivation();
		}

		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetAttackingEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());

		//Update Stamina Regen
		PlayerCharacter->UpdateStaminaRegen();
	}
}

void UGA_ManaPlayerSword_Attack_01::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		FGameplayTagContainer AttackTags;
		AttackTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
		ActorInfo->AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(AttackTags);

		APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());

		if (PlayerCharacter)
		{
			//Update Gameplay Effects
			if (!ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling"))))
			{
				ActorInfo->AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetFreeEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, ActorInfo->AbilitySystemComponent->MakeEffectContext());

			}

			//Update the anim instance
			UManaPlayerAnimInstance* AnimInstance = Cast<UManaPlayerAnimInstance>(PlayerCharacter->GetMesh()->GetAnimInstance());
			if (AnimInstance)
			{
				AnimInstance->SetIsAttacking(false);
				AnimInstance->Montage_Stop(0.1f, PlayerCharacter->GetAttackMontage());

			}

			//Update Stamina Regen
			PlayerCharacter->UpdateStaminaRegen();
		}
	}
}

void UGA_ManaPlayerSword_Attack_01::OnGameplayEventReceived(FGameplayEventData Payload)
{
	if (DamageEffectClass && Payload.Target)
	{
		const AGASManaCharacter* TargetGASCharacter = Cast<AGASManaCharacter>(Payload.Target.Get());
		AGASManaCharacter* MutableTargetGASCharacter = const_cast<AGASManaCharacter*>(TargetGASCharacter);
		if (TargetGASCharacter)
		{
			UAbilitySystemComponent* TargetASC = TargetGASCharacter->GetAbilitySystemComponent();
			if (TargetASC)
			{
				//Create a gameplay effect spec
				FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
				FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(DamageEffectClass, 1, EffectContext);
				if (SpecHandle.IsValid())
				{
					TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}

			//After applying damage, now play a hit react montage (change this function later to run a function that returns a montage depending on direction!)
			MutableTargetGASCharacter->PlayAnimMontage(TargetGASCharacter->GetHitReactMontage());

			//Uncomment if debug strings are needed
			/*
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("Payload.Target: %s"), *Payload.Target->GetName()));
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("Damage Class: %s"), *DamageEffectClass->GetName()));
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Gameplay Effect Applied!"));
			}
			*/
		}
	}
}

void UGA_ManaPlayerSword_Attack_01::OnMontageEnded()
{
	// End the ability (get the current context)
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}