// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerJump.h"
#include "Effect/GE_ManaPlayerAirborne.h"
#include "PlayerManaCharacter.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionJumpForce.h"

UGA_ManaPlayerJump::UGA_ManaPlayerJump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;	
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.Jump")));
	SetAssetTags(Tags);

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
}

void UGA_ManaPlayerJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{

	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		//First, make the character jump
		APlayerManaCharacter* Character = CastChecked<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());

		
		Character->ACharacter::Jump();

		//UAbilityTask_ApplyRootMotionJumpForce* JumpTask = UAbilityTask_ApplyRootMotionJumpForce::ApplyRootMotionJumpForce(this, NAME_None, Character->GetActorRotation(), 0.f, 300.f, 1.25f, .5f, true, ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity, FVector::ZeroVector, 0.f, nullptr, Character->GetJumpCurve());

		//if (GEngine) {
		//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Jump Ability Activated!");
		//}

		//if (JumpTask)
		//{
			//JumpTask->OnFinish.AddDynamic(this, &UGA_ManaPlayerJump::EndAbility);//, <bool, bool, FVector>);
			//JumpTask->ReadyForActivation();
		//}

		//Next, Commit the ability
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		//Apply the airborne gameplay effect to owner
		if (ActorInfo->AbilitySystemComponent.IsValid())
		{
			TSubclassOf<UGameplayEffect> AirborneEffect = UGE_ManaPlayerAirborne::StaticClass();
			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectToSelf(AirborneEffect.GetDefaultObject(), 1.0f, ActorInfo->AbilitySystemComponent->MakeEffectContext());

			//Remove the grounded effect by tag
			FGameplayTagContainer GroundedTags;
			GroundedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsGrounded")));
			ActorInfo->AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(GroundedTags);
		}

		//End Ability after logic is completed
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
	}
}

/*

bool UGA_ManaPlayerJump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get(), ECastCheckedType::NullAllowed);
	return Character && Character->CanJump();
}

void UGA_ManaPlayerJump::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

// Epic's comment -- Thank you epic
/**
 *	Canceling an non instanced ability is tricky. Right now this works for Jump since there is nothing that can go wrong by calling
 *	StopJumping() if you aren't already jumping. If we had a montage playing non instanced ability, it would need to make sure the
 *	Montage that *it* played was still playing, and if so, to cancel it. If this is something we need to support, we may need some
 *	light weight data structure to represent 'non intanced abilities in action' with a way to cancel/end them.
 */

/*

void UGA_ManaPlayerJump::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UGA_ManaPlayerJump::CancelAbility, Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
	

	ACharacter* Character = CastChecked<ACharacter>(ActorInfo->AvatarActor.Get());
	Character->StopJumping();
}

*/