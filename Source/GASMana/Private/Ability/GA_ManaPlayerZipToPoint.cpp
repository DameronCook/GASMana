// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerZipToPoint.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionMoveToActorForce.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "PlayerManaCharacter.h"
#include "Components/AC_HookShot.h"
#include "Actors/ManaHookParent.h"
#include "GameFramework/CharacterMovementComponent.h"


UGA_ManaPlayerZipToPoint::UGA_ManaPlayerZipToPoint()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.ZipToPoint")));
	SetAssetTags(Tags);

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsHooked")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsMantling")));
}

void UGA_ManaPlayerZipToPoint::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
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

	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	if (PlayerCharacter && AbilitySystemComponent)
	{
		PlayerCharacter->AddActorWorldOffset(FVector(0.f, 0.f, 50.f));
		PlayerCharacter->SetZipToPointAbility(this);
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetZipToPointEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());

		//UAbilityTask_ApplyRootMotionConstantForce* RootMotionTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, NAME_None, Direction, Strength, Duration, false, StrengthOverTime, VelocityOnFinishMode, SetVelocityOnFinish, false, true);

		//Apply the root motion task
		UAC_HookShot* HookShot = PlayerCharacter->GetHookShot();
		AManaHookParent* Hook = HookShot->GetCurrentTarget();
		FVector OffsetAlignment = FVector::ZeroVector;
		ERootMotionMoveToActorTargetOffsetType OffsetType = ERootMotionMoveToActorTargetOffsetType::AlignToWorldSpace;
		float Duration = .45f;
		UCurveFloat* CurveFloatHorizontal = PlayerCharacter->GetZipToPointCurveFloat();
		UCurveFloat* CurveFloatVertical = PlayerCharacter->GetZipToPointCurveFloat();
		bool bNewMovementMode = false;
		EMovementMode MoveMode = EMovementMode::MOVE_None;
		bool bRestrictSpeedToExpected = true;
		UCurveVector* OffsetCurveVector = nullptr;
		UCurveFloat* TimeMappingCurve = nullptr;
		ERootMotionFinishVelocityMode VelocityFinishMode = ERootMotionFinishVelocityMode::SetVelocity;
		FVector SetVelocityOnFinish = FVector(0, 0, 750.f);
		float ClampVelocityOnFinish = 0.f;
		bool bDisableDestinationReachInterrupt = false;

		RootMotionTask = UAbilityTask_ApplyRootMotionMoveToActorForce::ApplyRootMotionMoveToActorForce(this, NAME_None, Hook, OffsetAlignment, OffsetType, Duration, CurveFloatHorizontal, CurveFloatVertical, bNewMovementMode, MoveMode, bRestrictSpeedToExpected, OffsetCurveVector, TimeMappingCurve, VelocityFinishMode, SetVelocityOnFinish, ClampVelocityOnFinish, bDisableDestinationReachInterrupt);
	
		if (RootMotionTask)
		{
			RootMotionTask->OnFinished.AddDynamic(this, &UGA_ManaPlayerZipToPoint::ReachedDestination);//, <bool, bool, FVector>);
			RootMotionTask->ReadyForActivation();
		}

		//Apply the Anim Montage
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PlayerCharacter->GetZipToPointMontage(), 1.0f, NAME_None, true, 0.0f);

		if (MontageTask)
		{
			MontageTask->ReadyForActivation();
		}
	}

}

void UGA_ManaPlayerZipToPoint::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString("End Ability Called! Woo!"));


	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	if (PlayerCharacter && AbilitySystemComponent)
	{
		PlayerCharacter->SetZipToPointAbility(nullptr);
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetFreeEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetAirborneEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		FGameplayTagContainer Tags;
		Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsZipToPoint")));
		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Tags);

	}

	UCharacterMovementComponent* CharMove = PlayerCharacter->GetCharacterMovement();

	if (CharMove)
	{

		//Launch Character FIRST before we clear out the current target .
		//Launch in direction of target for added boost to get over small gaps... Listen it makes the game feel better
		FVector LaunchDir = PlayerCharacter->GetHookShot()->GetCharacterInitDir();
		LaunchDir *= 1100.f;
		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, LaunchDir.ToString());

		CharMove->Launch(FVector(LaunchDir.X, LaunchDir.Y, 950.f));

		//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange, "END GRAPPLE CALLED LAUNCH CHARACTER BITCH");

	}

}

void UGA_ManaPlayerZipToPoint::ReachedDestination(bool DestinationReached, bool TimedOut, FVector FinalTargetLocation)
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_ManaPlayerZipToPoint::CancelMotionTask()
{
	if (RootMotionTask)
	{
		RootMotionTask->EndTask();
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}