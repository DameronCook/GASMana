// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerWallJump.h"
#include "../../Public/PlayerManaCharacter.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "GameFramework/CharacterMovementComponent.h"

UGA_ManaPlayerWallJump::UGA_ManaPlayerWallJump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.WallJump")));
	SetAssetTags(Tags);

	//Required Tags
	ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallJumping")));
}

void UGA_ManaPlayerWallJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	if (PlayerCharacter && AbilitySystemComponent)
	{
		UAC_WallRun* WallRunComponent = PlayerCharacter->GetWallRun();
		
		if (WallRunComponent)
		{

			AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetAirborneEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
			WallJumpEffectHandle = AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetWallJumpEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());

			//Cancel the wall run ability
			FGameplayTag WallRunTag = FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning"));
			FGameplayTagContainer WallRunTags;
			WallRunTags.AddTag(WallRunTag);
			AbilitySystemComponent->RemoveActiveGameplayEffect(PlayerCharacter->GetWallRunAbility()->GetWallRunEffectHandle());

			PlayerCharacter->GetWallRunAbility()->OnWallRunFinished();
			PlayerCharacter->SwitchCamaeraState(ECameraState::E_WallRun);

			FVector Direction;
			FVector InputDirection = PlayerCharacter->GetCachedInputDirection().GetSafeNormal();
			FVector WallRunDirection = WallRunComponent->GetWallRunDirection().GetSafeNormal();
			FVector WallNormal = WallRunComponent->GetWallRunImpactNormal().GetSafeNormal();
			FVector Forward = PlayerCharacter->GetActorForwardVector().GetSafeNormal();
			FVector Bisector = (WallNormal + Forward).GetSafeNormal();

			float WallDot = FVector::DotProduct(InputDirection, WallNormal);

			if (WallDot > 0.1f)
			{
				Direction = Bisector + InputDirection;

				//if (GEngine)
				//{
				//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Used Input!"));
				//}
			}
			else
			{
				Direction = Bisector;

				/*		if (GEngine)
						{
							GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Didin't use Input!"));
						}*/
			}

			Direction.Z = 0.45f;

			float Strength = 600.f; // Adjust as needed
			float Duration = 0.25f; // Duration in seconds
			bool bIsAdditive = false;
			bool bDisableDestinationReachedInterrupt = false;
			UCurveFloat* StrengthOverTime = WallRunComponent->GetWallJumpCurveFloat();
			ERootMotionFinishVelocityMode VelocityOnFinishMode = ERootMotionFinishVelocityMode::MaintainLastRootMotionVelocity;
			FVector SetVelocityOnFinish = PlayerCharacter->GetCharacterMovement()->Velocity;
			float ClampVelocityOnFinish = 0.f;

			UAbilityTask_ApplyRootMotionConstantForce* RootMotionTask = UAbilityTask_ApplyRootMotionConstantForce::ApplyRootMotionConstantForce(this, NAME_None, Direction, Strength, Duration, false, StrengthOverTime, VelocityOnFinishMode, SetVelocityOnFinish, false, false);

			if (RootMotionTask)
			{
				RootMotionTask->OnFinish.AddDynamic(this, &UGA_ManaPlayerWallJump::OnMotionTaskEnded);
				RootMotionTask->ReadyForActivation();
			}
		}
	}

}

void UGA_ManaPlayerWallJump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);



	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();


	PlayerCharacter->SwitchCamaeraState(ECameraState::E_Default);

	if (AbilitySystemComponent)
	{
		//AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetAirborneEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());

		AbilitySystemComponent->RemoveActiveGameplayEffect(WallJumpEffectHandle);
	}
}

void UGA_ManaPlayerWallJump::OnMotionTaskEnded()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

