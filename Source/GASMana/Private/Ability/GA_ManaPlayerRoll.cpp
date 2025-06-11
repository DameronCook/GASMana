// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerRoll.h"
#include "../../Public/PlayerManaCharacter.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_ManaPlayerRoll::UGA_ManaPlayerRoll()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.Roll")));
	SetAssetTags(Tags);

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));
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
		FVector Direction = PlayerCharacter->GetLastMovementInputVector().GetSafeNormal();;

		if (Direction.Length() == 0)
		{
			Direction = PlayerCharacter->GetActorForwardVector().GetSafeNormal();

		}
			
		float Strength = 500.f; // Adjust as needed
		float Duration = 0.5f; // Duration in seconds
		bool bIsAdditive = false;
		bool bDisableDestinationReachedInterrupt = false;
		UCurveFloat* StrengthOverTime = PlayerCharacter->GetDiveRollCurveFloat();
		ERootMotionFinishVelocityMode VelocityOnFinishMode = ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity;
		FVector SetVelocityOnFinish = FVector::ZeroVector;
		float ClampVelocityOnFinish = 0.f;

		UAbilityTask_ApplyRootMotionConstantForce* RootMotionTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, NAME_None, Direction, Strength, Duration, false, StrengthOverTime, VelocityOnFinishMode, SetVelocityOnFinish, false, true);

		if (RootMotionTask)
		{
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
}


void UGA_ManaPlayerRoll::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		FGameplayTagContainer RollingTags;
		RollingTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
		ActorInfo->AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(RollingTags);

		APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());

		if (PlayerCharacter)
		{
			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetFreeEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, ActorInfo->AbilitySystemComponent->MakeEffectContext());
		}
	}
}

void UGA_ManaPlayerRoll::OnMontageEnded()
{
	// End the ability (get the current context)
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}