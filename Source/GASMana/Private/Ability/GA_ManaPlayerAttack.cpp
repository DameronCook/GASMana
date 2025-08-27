#include "Ability/GA_ManaPlayerAttack.h"
#include "../../Public/PlayerManaCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "ManaPlayerAnimInstance.h"
#include "../../GASManaCharacter.h"

UGA_ManaPlayerAttack::UGA_ManaPlayerAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.Action.Attack")));
	SetAssetTags(Tags);

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsEquipping")));
}

void UGA_ManaPlayerAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//Next, Commit the ability
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Couldn't Commit Attack!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	AGASManaCharacter* Character = Cast<AGASManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	
	if (Character) {

		Character->SetAttackAbility(this);

		// Play the montage and bind delegates
		if (Character->GetCurrentAttackMontage() && ActorInfo->AvatarActor.IsValid())
		{
			Character->GetEquipmentState() == EEquipmentState::EES_Unequipped ? Character->SetEquipmentState(EEquipmentState::EES_EquippedOneHandedWeapon) : Character->SetEquipmentState(Character->GetEquipmentState());

			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Next Combo Section: %s"), *PlayerCharacter->GetNextAttackMontageSection().ToString()));

			if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, Character->GetCurrentAttackMontage(), 1.0f, Character->GetNextAttackMontageSection(), false, 0.0f))
			{
				MontageTask->OnCompleted.AddDynamic(this, &UGA_ManaPlayerAttack::OnMontageEnded);
				MontageTask->OnInterrupted.AddDynamic(this, &UGA_ManaPlayerAttack::OnMontageEnded);
				MontageTask->OnCancelled.AddDynamic(this, &UGA_ManaPlayerAttack::OnMontageEnded);
				MontageTask->OnBlendOut.AddDynamic(this, &UGA_ManaPlayerAttack::OnMontageEnded);
				MontageTask->ReadyForActivation();
			}
		}
		else
		{
			// If no montage, just end ability immediately
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}

		AbilitySystemComponent->ApplyGameplayEffectToSelf(Character->GetAttackingEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());

		//Update Stamina Regen if player
		if (const APlayerManaCharacter* PlayerChar = Cast<APlayerManaCharacter>(Character))
		{
			PlayerChar->UpdateStaminaRegen();
		}
	}
}

void UGA_ManaPlayerAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	//GEngine->AddOnScreenDebugMessage(4, 3.f, FColor::Purple, "EndAbility Called!");

	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		FGameplayTagContainer AttackTags;
		AttackTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
		ActorInfo->AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(AttackTags);
		
		if (AGASManaCharacter* Character = Cast<AGASManaCharacter>(ActorInfo->AvatarActor.Get()))
		{
			Character->SetAttackAbility(nullptr);

			//Update Stamina Regen if player
			if (const APlayerManaCharacter* PlayerChar = Cast<APlayerManaCharacter>(Character))
			{
				//Apparently we don't need to do anything with the anim instance?
				PlayerChar->UpdateStaminaRegen();
				//UManaPlayerAnimInstance* AnimInstance = Cast<UManaPlayerAnimInstance>(GetMesh()->GetAnimInstance());
				//AnimInstance->SetIsAttacking(false);
			}
		}
	}
}

void UGA_ManaPlayerAttack::EndAbilityAndListenForCombo()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	//GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Purple, "EndAbilityAndListenForCombo Called!");
}

void UGA_ManaPlayerAttack::OnMontageEnded()
{
	// End the ability (get the current context)
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}