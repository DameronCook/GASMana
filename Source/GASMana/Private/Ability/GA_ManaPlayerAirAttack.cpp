// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerAirAttack.h"
#include "PlayerManaCharacter.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionJumpForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"

UGA_ManaPlayerAirAttack::UGA_ManaPlayerAirAttack()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.AirAttack")));
	SetAssetTags(Tags);

	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirAttacking")));

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsGrounded")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsMantling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsBlocking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsSwinging")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsZipToPoint")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsHooked")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsEquipping")));
}

void UGA_ManaPlayerAirAttack::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//Next, Commit the ability
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Couldn't Commit Attack!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	
	if (UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
	{
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetAirAttackEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.IsFree")));
	}

	if (PlayerCharacter)
	{
		PlayerCharacter->SetAirAttackAbility(this);

		if (PlayerCharacter->EquipmentState == EEquipmentState::EES_Unequipped)
		{
			PlayerCharacter->AttachWeaponToHand();
		}

		FGameplayTag FreeTag;
		FreeTag = FGameplayTag::RequestGameplayTag(FName("Character.IsFree"));

		while (ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FreeTag))
		{
			//Grant the player a tag so that they can move again in case this was blocked before
			PlayerCharacter->RemoveFreeTag();
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Removing Gameplay tag!");
		}

		//First, play the animation
		UAnimMontage* MontageToPlay = PlayerCharacter->GetAirAttackMontage();
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay, 1.0f, "Start", false, 0.0f);
		
		if (MontageTask)
		{
			MontageTask->ReadyForActivation();
		}

		//Next, apply the root motion jump force to give a small upward impulse then bring it back down on to the ground
		FVector Direction = PlayerCharacter->GetCachedInputDirection();
		if (Direction.IsNearlyZero())
		{
			// If no input, roll on actor forward (maybe change this to camera forward...?)
			Direction = PlayerCharacter->GetActorForwardVector().GetSafeNormal();
		}
		FRotator Rotation = Direction.Rotation();
		PlayerCharacter->SetActorRotation(Rotation, ETeleportType::None);

		float Distance = 300.f;
		float Height = 150.f;
		float Duration = 0.3f;
		float MinimumLandedTriggerTime = 10.f;
		bool bFinishOnLanded = true;
		UCurveVector* PathOffsetCurve = nullptr;
		UCurveFloat* TimeMappingCurve = nullptr;

		UAbilityTask_ApplyRootMotionJumpForce* JumpForce = UAbilityTask_ApplyRootMotionJumpForce::ApplyRootMotionJumpForce(this, NAME_None, Rotation, Distance, Height, Duration, MinimumLandedTriggerTime, bFinishOnLanded, ERootMotionFinishVelocityMode::SetVelocity, FVector::ZeroVector, 0.f, PathOffsetCurve, TimeMappingCurve);

		if (JumpForce)
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Jump Force Being Applied");
			JumpForce->OnFinish.AddDynamic(this, &UGA_ManaPlayerAirAttack::OnJumpLanded);
			JumpForce->OnLanded.AddDynamic(this, &UGA_ManaPlayerAirAttack::OnJumpLanded);
			JumpForce->ReadyForActivation();
		}
	}
}

void UGA_ManaPlayerAirAttack::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);


	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(CurrentActorInfo->AvatarActor.Get());

	if (PlayerCharacter)
	{
		PlayerCharacter->SetAirAttackAbility(nullptr);
	}
}

void UGA_ManaPlayerAirAttack::OnJumpLanded()
{
	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(CurrentActorInfo->AvatarActor.Get());

	if (PlayerCharacter)
	{
		UAnimMontage* MontageToPlay = PlayerCharacter->GetAirAttackMontage();
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, MontageToPlay, 1.0f, "End", false, 0.0f);

		if (MontageTask)
		{
			MontageTask->ReadyForActivation();
		}
	}

	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);

}


