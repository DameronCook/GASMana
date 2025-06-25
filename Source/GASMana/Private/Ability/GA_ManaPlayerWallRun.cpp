// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerWallRun.h"
#include "../../Public/PlayerManaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "ManaPlayerAnimInstance.h"


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

bool UGA_ManaPlayerWallRun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if (const UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
	{
		float CurrentMana = AbilitySystemComponent->GetNumericAttribute(UManaAttributeSet::GetManaAttribute());
		if (CurrentMana <= 0.0f)
		{
			return false;
		}
	}
	return true;
}


void UGA_ManaPlayerWallRun::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	UCharacterMovementComponent* CharacterMovement = PlayerCharacter->GetCharacterMovement();


	if (PlayerCharacter && AbilitySystemComponent && CharacterMovement)
	{
		PlayerCharacter->SetWallRunAbility(this);
		PlayerCharacter->SetWallRunCameraState();
		/*
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Wall Run ability Actiivated!");
		}
		*/

		CharacterMovement->bOrientRotationToMovement = false;

		//Apply Gameplay Effects
		WallRunEffectHandle = AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetWallRunEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		ManaDrainEffectHandle = PlayerCharacter->GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(PlayerCharacter->GetWallRunDrainEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
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
	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Wall Run Finished!");
	//}

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

			PlayerCharacter->SetDefaultCameraState();



			FVector WallNormal = PlayerCharacter->GetWallRunImpactNormal().GetSafeNormal();
			FVector Forward = PlayerCharacter->GetActorForwardVector().GetSafeNormal();
			FVector Bisector = (WallNormal + Forward).GetSafeNormal();

			//Set the players rotation
			FRotator JumpYaw = FRotator(0.f, Bisector.Rotation().Yaw, 0.f);
			PlayerCharacter->SetActorRotation(JumpYaw);




			if (UManaPlayerAnimInstance* AnimInstance = Cast<UManaPlayerAnimInstance>(PlayerCharacter->GetMesh()->GetAnimInstance()))
			{
				AnimInstance->Montage_Play(PlayerCharacter->GetWallJumpMontage());
			}

			if (UAbilitySystemComponent* AbilitySystem = PlayerCharacter->GetAbilitySystemComponent())
			{
				AbilitySystem->RemoveActiveGameplayEffect(ManaDrainEffectHandle);
				AbilitySystem->RemoveActiveGameplayEffect(WallRunEffectHandle);

			}
			ActorInfo->AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetFreeEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, ActorInfo->AbilitySystemComponent->MakeEffectContext());

			UCharacterMovementComponent* CharacterMovement = PlayerCharacter->GetCharacterMovement();
			if (CharacterMovement)
			{
				CharacterMovement->bOrientRotationToMovement = true;
				CharacterMovement->GravityScale = PlayerCharacter->GetOriginalGravityScale();

				//Move the player a little bit away from the wall too so they won't immediately collide with the wall, restarting a run
				PlayerCharacter->LaunchCharacter(Bisector * 500, true, false);
			}
		}
	}
}