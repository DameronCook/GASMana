// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerWallRun.h"
#include "../../Public/PlayerManaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_ApplyRootMotionConstantForce.h"
#include "AbilityTasks/ApplyRootMotionDynamicForce.h"

UGA_ManaPlayerWallRun::UGA_ManaPlayerWallRun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.WallRun")));
	SetAssetTags(Tags);

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsGrounded")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));
}

void UGA_ManaPlayerWallRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	UCharacterMovementComponent* CharacterMovement = PlayerCharacter->GetCharacterMovement();


	if (PlayerCharacter && AbilitySystemComponent && CharacterMovement)
	{
		PlayerCharacter->SetWallRunAbility(this);
		PlayerCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(PlayerCharacter->GetWallRunDrainEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		/*
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Wall Run ability Actiivated!");
		}
		*/

		CharacterMovement->bOrientRotationToMovement = false;

		//Apply Gameplay Effects
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetWallRunEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetBlockMovementEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.IsFree")));
		AbilitySystemComponent->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));



		FVector WallRunDirection = PlayerCharacter->GetWallRunDirection();
		FRotator WallRunRotation = WallRunDirection.Rotation();
		CharacterMovement->GravityScale = 0.0f;
		CharacterMovement->Velocity.Z = 0.0f;

		PlayerCharacter->SetActorRotation(WallRunRotation);

		//Apply the Montage
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(this, NAME_None, PlayerCharacter->GetWallRunMontage(), 1.0f, NAME_None, true, 0.0f);
		
		if (MontageTask)
		{
			//MontageTask->OnCompleted.AddDynamic(this, &UGA_ManaPlayerWallRun::OnWallRunFinished);
			MontageTask->OnInterrupted.AddDynamic(this, &UGA_ManaPlayerWallRun::OnWallRunFinished);
			MontageTask->OnCancelled.AddDynamic(this, &UGA_ManaPlayerWallRun::OnWallRunFinished);
			MontageTask->ReadyForActivation();
		}
	}
}

void UGA_ManaPlayerWallRun::OnWallRunFinished()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Wall Run Finished!");
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UGA_ManaPlayerWallRun::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	if (ActorInfo && ActorInfo->AbilitySystemComponent.IsValid())
	{
		//We should only remove the tag if the player JUMPS out of a wallrun. If we remove the wall run tag here, they'll just start running again and again and again and again and again and again
		//FGameplayTagContainer WallRunTags;
		//WallRunTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));
		//ActorInfo->AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(WallRunTags);

		APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());

		if (PlayerCharacter)
		{
			PlayerCharacter->SetWallRunAbility(nullptr);

			if (UAbilitySystemComponent* AbilitySystem = PlayerCharacter->GetAbilitySystemComponent())
			{
				FActiveGameplayEffectHandle ManaDrainEffectHandle = FActiveGameplayEffectHandle::GenerateNewHandle(AbilitySystem);
				AbilitySystem->RemoveActiveGameplayEffect(ManaDrainEffectHandle);
			}
			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetFreeEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, ActorInfo->AbilitySystemComponent->MakeEffectContext());

			UCharacterMovementComponent* CharacterMovement = PlayerCharacter->GetCharacterMovement();
			if (CharacterMovement)
			{
				CharacterMovement->bOrientRotationToMovement = true;
				CharacterMovement->GravityScale = PlayerCharacter->GetOriginalGravityScale();

			}
		}
	}
}