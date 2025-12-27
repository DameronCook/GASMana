// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerLaunchUp.h"

#include "PlayerManaCharacter.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToActorForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Actors/ManaHookParent.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_ManaPlayerLaunchUp::UGA_ManaPlayerLaunchUp()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.LaunchUp")));
	SetAssetTags(Tags);

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsMantling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsHooked")));
}

void UGA_ManaPlayerLaunchUp::ActivateAbility(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Couldn't Commit Zip to Point!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString("Ability Activated!!"));

	PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	if (PlayerCharacter && AbilitySystemComponent)
	{
		PlayerCharacter->AddActorWorldOffset(FVector(0.f, 0.f, 50.f));
		PlayerCharacter->SetLaunchUpAbility(this);
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetLaunchUpEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		HookBlockEffectHandle = AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetHookBlockEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());

		//Apply the root motion task
		const UAC_HookShot* HookShot = PlayerCharacter->GetHookShot();
		AManaHookParent* Hook = HookShot->GetCurrentTarget();
		const FVector OffsetAlignment = FVector::ZeroVector;
		ERootMotionMoveToActorTargetOffsetType OffsetType = ERootMotionMoveToActorTargetOffsetType::AlignToWorldSpace;
		constexpr float Duration = .45f;
		UCurveFloat* CurveFloatHorizontal = PlayerCharacter->GetZipToPointCurveFloat();
		UCurveFloat* CurveFloatVertical = PlayerCharacter->GetZipToPointCurveFloat();
		constexpr bool bNewMovementMode = false;
		constexpr EMovementMode MoveMode = EMovementMode::MOVE_None;
		constexpr bool bRestrictSpeedToExpected = true;
		UCurveVector* OffsetCurveVector = nullptr;
		UCurveFloat* TimeMappingCurve = nullptr;
		constexpr ERootMotionFinishVelocityMode VelocityFinishMode = ERootMotionFinishVelocityMode::SetVelocity;
		const FVector SetVelocityOnFinish = FVector(0, 0, 750.f);
		constexpr float ClampVelocityOnFinish = 0.f;
		constexpr bool bDisableDestinationReachInterrupt = false;

		RootMotionTask = UAbilityTask_ApplyRootMotionMoveToActorForce::ApplyRootMotionMoveToActorForce(this, NAME_None, Hook, OffsetAlignment, OffsetType, Duration, CurveFloatHorizontal, CurveFloatVertical, bNewMovementMode, MoveMode, bRestrictSpeedToExpected, OffsetCurveVector, TimeMappingCurve, VelocityFinishMode, SetVelocityOnFinish, ClampVelocityOnFinish, bDisableDestinationReachInterrupt);
	
		if (RootMotionTask)
		{
			RootMotionTask->OnFinished.AddDynamic(this, &UGA_ManaPlayerLaunchUp::ReachedDestination);// <bool, bool, FVector>
			RootMotionTask->ReadyForActivation();
		}
		//Apply the Anim Montage
		if (UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PlayerCharacter->GetZipToPointMontage(), 1.0f, NAME_None, true, 0.0f))
		{
			MontageTask->ReadyForActivation();
		}
	}
}

void UGA_ManaPlayerLaunchUp::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	if (PlayerCharacter && AbilitySystemComponent)
	{
		PlayerCharacter->SetLaunchUpAbility(nullptr);
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetFreeEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetAirborneEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		FGameplayTagContainer Tags;
		Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsLaunchUp")));
		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Tags);
		AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.HookBlocked")));
	}
}

void UGA_ManaPlayerLaunchUp::EndLaunchUp()
{
	GetWorld()->GetTimerManager().ClearTimer(EndLaunchTimerHandle);
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_ManaPlayerLaunchUp::ReachedDestination(bool DestinationReached, bool TimedOut, FVector FinalTargetLocation)
{
	if (PlayerCharacter)
	{
		if (UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent())
		{
			ASC->RemoveActiveGameplayEffect(HookBlockEffectHandle);
			HookBlockEffectHandle.Invalidate();
		}
		if (DestinationReached)
		{
			if (UCharacterMovementComponent* CharMove = PlayerCharacter->GetCharacterMovement())
			{
				CharMove->Launch(FVector(0.f, 0.f, 2000.f));
				GetWorld()->GetTimerManager().SetTimer(EndLaunchTimerHandle, this, &UGA_ManaPlayerLaunchUp::EndLaunchUp, .7f, false);
			}
		}
	}
	//EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UGA_ManaPlayerLaunchUp::CancelMotionTask()
{
	if (RootMotionTask)
	{
		RootMotionTask->EndTask();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
