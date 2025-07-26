// Fill out your copyright notice in the Description page of Project Settings.

#include "Ability/GA_ManaPlayerSwingJump.h"
#include "Ability/GA_ManaPlayerSwing.h"
#include "../../Public/PlayerManaCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/AC_HookShot.h"
#include "Kismet/KismetMathLibrary.h"

UGA_ManaPlayerSwingJump::UGA_ManaPlayerSwingJump()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.SwingJump")));
	SetAssetTags(Tags);

	//Required Tags
	ActivationRequiredTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsSwinging")));

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsSwingJumping")));
}

void UGA_ManaPlayerSwingJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		if (GEngine)
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Couldn't Commit Swing Ability!"));
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Committed Swing Jump!"));

	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UCharacterMovementComponent* CharacterMovement = PlayerCharacter->GetCharacterMovement();
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	if (PlayerCharacter && AbilitySystemComponent && CharacterMovement)
	{
		AbilitySystemComponent->ApplyGameplayEffectToSelf(PlayerCharacter->GetSwingJumpEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
		
		if (PlayerCharacter->GetSwingAbility())
		{
			PlayerCharacter->GetSwingAbility()->CancelSwing();
		}

		UAC_HookShot* PlayerHook = PlayerCharacter->GetHookShot();
		
		if (PlayerHook)
		{
			FVector LaunchDir = PlayerHook->FindSwingLaunchForce(PlayerCharacter, 7.f, 7.f);
			LaunchDir = UKismetMathLibrary::Divide_VectorFloat(LaunchDir, PlayerCharacter->GetSwingSpeedBalancer());
			LaunchDir = UKismetMathLibrary::Multiply_VectorFloat(LaunchDir, CharacterMovement->GravityScale);

			PlayerCharacter->LaunchCharacter(FVector(LaunchDir.X, LaunchDir.Y, fmax(LaunchDir.Z, 700.f)), false, false);

			PlayerHook->EndGrapple();
			
		}

		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
}

void UGA_ManaPlayerSwingJump::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();

	if (AbilitySystemComponent)
	{
		FGameplayTagContainer Tags;
		Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsSwingJumping")));
		AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(Tags);
	}
}