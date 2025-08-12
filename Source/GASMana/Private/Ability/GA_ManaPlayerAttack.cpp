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
	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	
	if (PlayerCharacter) {

		PlayerCharacter->SetAttackAbility(this);

		// Play the montage and bind delegates
		if (PlayerCharacter->GetCurrentAttackMontage() && ActorInfo->AvatarActor.IsValid())
		{
			PlayerCharacter->EquipmentState == EEquipmentState::EES_Unequipped ? PlayerCharacter->EquipmentState = EEquipmentState::EES_EquippedOneHandedWeapon : PlayerCharacter->EquipmentState = PlayerCharacter->EquipmentState;

			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Next Combo Section: %s"), *PlayerCharacter->GetNextAttackMontageSection().ToString()));

			UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PlayerCharacter->GetCurrentAttackMontage(), 1.0f, PlayerCharacter->GetNextAttackMontageSection(), false, 0.0f);

			if (MontageTask)
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

		/*
		FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Character.Damaged"));

		UAbilityTask_WaitGameplayEvent* WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(this, EventTag, nullptr, true, true);

		if (WaitTask)
		{
			WaitTask->EventReceived.AddDynamic(this, &UGA_ManaPlayerAttack::OnGameplayEventReceived);
			WaitTask->ReadyForActivation();
		}
		*/

		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetAttackingEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());

		//Update Stamina Regen
		PlayerCharacter->UpdateStaminaRegen();
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
		
		FGameplayTag FreeTag = FGameplayTag::RequestGameplayTag(FName("Character.IsFree"));
		
		FGameplayTag RollingTag = FGameplayTag::RequestGameplayTag(FName("Player.IsRolling"));

		/*
		if (!ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FreeTag))
		{
			if (!ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(RollingTag))
			{
				//Grant the player a tag so that they can move again in case this was blocked before
				ActorInfo->AbilitySystemComponent->AddLooseGameplayTag(FreeTag);
				//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Adding Gameplay tag!");
			}
		}
		*/
		
		if (APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get()))
		{
			PlayerCharacter->SetAttackAbility(nullptr);

			//Update the anim instance
			if (UManaPlayerAnimInstance* AnimInstance = Cast<UManaPlayerAnimInstance>(PlayerCharacter->GetMesh()->GetAnimInstance()))
			{
				//TODO: Need to set IsAttacking to false whenever the montage has completely ended... haven't used the IsAttacking variable yet though
				//AnimInstance->SetIsAttacking(false);
			}

			//Update Stamina Regen
			PlayerCharacter->UpdateStaminaRegen();
		}
	}
}

/*
void UGA_ManaPlayerAttack::OnGameplayEventReceived(FGameplayEventData const Payload)
{
	if (DamageEffectClass && Payload.Target)
	{
		const AGASManaCharacter* TargetGASCharacter = Cast<AGASManaCharacter>(Payload.Target.Get());
		AGASManaCharacter* MutableTargetGASCharacter = const_cast<AGASManaCharacter*>(TargetGASCharacter);
		if (TargetGASCharacter)
		{
			if (UAbilitySystemComponent* TargetASC = TargetGASCharacter->GetAbilitySystemComponent())
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
			
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("Payload.Target: %s"), *Payload.Target->GetName()));
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, FString::Printf(TEXT("Damage Class: %s"), *DamageEffectClass->GetName()));
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Gameplay Effect Applied!"));
			}
			
		}
	}
}
*/

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