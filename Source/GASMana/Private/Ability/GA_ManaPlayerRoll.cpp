// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerRoll.h"
#include "../../Public/PlayerManaCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"

UGA_ManaPlayerRoll::UGA_ManaPlayerRoll()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.Roll")));
	SetAssetTags(Tags);

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));
}

void UGA_ManaPlayerRoll::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//Next, Commit the ability
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Couldn't Commit Roll!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	if (PlayerCharacter && PlayerCharacter->GetRollingEffectClass() && PlayerCharacter->GetBlockMovementEffectClass() && AbilitySystemComponent)
	{
		PlayerCharacter->InstantlyUnequipGear();
		//Cancel all abilities with the Player.IsAttacking tag
		FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking"));
		FGameplayTagContainer CancelTags;
		CancelTags.AddTag(AttackTag);
		AbilitySystemComponent->CancelAbilities(&CancelTags, nullptr, this);

		FVector Direction = PlayerCharacter->GetCachedInputDirection();
		if (Direction.IsNearlyZero())
		{
			// If no input, roll on actor forward (maybe change this to camera forward...?)
			Direction = PlayerCharacter->GetActorForwardVector().GetSafeNormal();
		}

		FRotator PlayerRotation = Direction.Rotation();

		PlayerCharacter->SetActorRotation(PlayerRotation, ETeleportType::None);
			
		float Strength = 500.f;
		float Duration = 0.5f; 
		bool bIsAdditive = false;
		bool bDisableDestinationReachedInterrupt = false;
		UCurveFloat* StrengthOverTime = PlayerCharacter->GetDiveRollCurveFloat();
		ERootMotionFinishVelocityMode VelocityOnFinishMode = ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity;
		FVector SetVelocityOnFinish = FVector::ZeroVector;
		float ClampVelocityOnFinish = 0.f;

		UAbilityTask_ApplyRootMotionConstantForce* RootMotionTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, NAME_None, Direction, Strength, Duration, false, StrengthOverTime, VelocityOnFinishMode, SetVelocityOnFinish, false, true);

		if (RootMotionTask)
		{
			RootMotionTask->OnFinish.AddDynamic(this, &UGA_ManaPlayerRoll::OnMotionTaskEnded);
			RootMotionTask->ReadyForActivation();
			//Now add a delegate to the root motion task that grants the player the "IsFree" tag again so they can move around a bit before the animation is over.
		}
		
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetRollingEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetBlockMovementEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
	}

	// Play the montage and bind delegates
	if (PlayerCharacter->GetRollMontage() && ActorInfo->AvatarActor.IsValid())
	{

		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PlayerCharacter->GetRollMontage(), 1.0f, NAME_None, false, 0.0f);

		if (MontageTask)
		{
			MontageTask->OnCompleted.AddDynamic(this, &UGA_ManaPlayerRoll::OnMontageEnded);
			MontageTask->OnInterrupted.AddDynamic(this, &UGA_ManaPlayerRoll::OnMontageEnded);
			MontageTask->OnCancelled.AddDynamic(this, &UGA_ManaPlayerRoll::OnMontageEnded);
			MontageTask->ReadyForActivation();
		}
	}
	else
	{
		// If no montage, just end ability immediately
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}

	//Update Stamina Regen
	PlayerCharacter->UpdateStaminaRegen();
}


void UGA_ManaPlayerRoll::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		FGameplayTagContainer RollingTags;
		RollingTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
		RollingTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
		ActorInfo->AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(RollingTags);

		

		APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());

		if (PlayerCharacter)
		{
			PlayerCharacter->UpdateStaminaRegen();
			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetFreeEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, ActorInfo->AbilitySystemComponent->MakeEffectContext());
			ActorInfo->AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.IsEquipping")));
		}
	}
}

void UGA_ManaPlayerRoll::OnMontageEnded()
{
	// End the ability (get the current context)
	//EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}


void UGA_ManaPlayerRoll::OnMotionTaskEnded()
{
	//Allow the player to move again by applying the free efect class
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}