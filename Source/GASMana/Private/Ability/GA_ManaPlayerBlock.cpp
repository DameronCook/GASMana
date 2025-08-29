// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerBlock.h"
#include "PlayerManaCharacter.h"
#include "ManaPlayerAnimInstance.h"
#include "Actors/BaseManaEnemy.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

UGA_ManaPlayerBlock::UGA_ManaPlayerBlock()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.Block")));
	SetAssetTags(Tags);

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsBlocking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirAttacking")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.IsEquipping")));
}


TArray<class AActor*> UGA_ManaPlayerBlock::FindAllActorsInRange(const APlayerManaCharacter* PlayerCharacter,
                                                                const float Radius) const
{
	const UObject* WorldContextObject = GetWorld();

	const FVector SpherePos = PlayerCharacter->GetActorLocation();
	
	// Set what actors to seek out from its collision channel
	TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
	TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	const TSubclassOf<AActor> SeekClass;

	const TArray<AActor*> ActorsToIgnore;

	TArray<AActor*> OutActors;
	
	UKismetSystemLibrary::SphereOverlapActors(WorldContextObject, SpherePos, Radius, TraceObjectTypes, SeekClass, ActorsToIgnore, OutActors);

	return OutActors;
}

bool UGA_ManaPlayerBlock::DrawLineToTarget(const ACharacter* Character, const AActor* OverlappedActor) const
{
	FHitResult OutHitLine;

	if (!OverlappedActor)
	{
		return false;
	}
	// Line parameters
	const FVector LineStartLocation = Character->GetActorLocation();
	const FVector LineEndLocation = OverlappedActor->GetActorLocation() + FVector(0.f, 0.f, 25.f);

	//Trace Parameters
	//bool bLineTraceComplex = false;
	FCollisionQueryParams LineQueryParams;
	LineQueryParams.AddIgnoredActor(Character);
	constexpr ECollisionChannel CollisionChannel = ECollisionChannel::ECC_Pawn;
	GetWorld()->LineTraceSingleByChannel(OutHitLine, LineStartLocation, LineEndLocation, CollisionChannel, LineQueryParams);

	//DrawDebugLine(GetWorld(), LineStartLocation, LineEndLocation, FColor::Red);

	return OutHitLine.GetActor() == OverlappedActor;
}

float UGA_ManaPlayerBlock::CalculateAngleToTarget(ACharacter* Character, const AActor* OverlappedActor)
{
	if (const APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(Character))
	{
		if (const UCameraComponent* Camera = PlayerCharacter->GetFollowCamera())
		{
			const FVector CameraForward = Camera->GetForwardVector();

			FVector DirectionToTarget = OverlappedActor->GetActorLocation() - Character->GetActorLocation();
			DirectionToTarget.Normalize();

			const float Angle = acosf(FVector::DotProduct(DirectionToTarget, CameraForward));
			return Angle;
		}
	}
	return BIG_NUMBER;
}

void UGA_ManaPlayerBlock::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo
                                          ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	//Next, Commit the ability
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get());
	UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get();
	if (PlayerCharacter && PlayerCharacter->GetBlockingEffectClass() && AbilitySystemComponent)
	{	
		AbilitySystemComponent->ApplyGameplayEffectToSelf(
			PlayerCharacter->GetBlockingEffectClass()->GetDefaultObject<UGameplayEffect>(),
			1.0f,
			AbilitySystemComponent->MakeEffectContext()
		);

		if (PlayerCharacter->GetLeftHandEquipment())
		{
			//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Orange, "Left hand equipment found");

			if (PlayerCharacter->GetEquipmentState() == EEquipmentState::EES_Unequipped)
			{	
				PlayerCharacter->AttachWeaponToHand();
			}
			PlayerCharacter->Blocking();
		}
		
		//Update Stamina Regen
		PlayerCharacter->UpdateStaminaRegen();
		float BestAngle = BIG_NUMBER;
		AActor* BestActor = nullptr;
		for (TArray<AActor*> OutActors = FindAllActorsInRange(PlayerCharacter, 5000.f); AActor* Actor : OutActors)
		{
			if (Actor != PlayerCharacter)
			{
				//GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Orange, FString::Printf(TEXT("Actor's found: %d"), i));
				if (Cast<IIFocusableInterface>(Actor))
				{
					if (DrawLineToTarget(PlayerCharacter, Actor))
					{
						const float CurrentAngle = CalculateAngleToTarget(PlayerCharacter, Actor);
						//GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Orange, FString::Printf(TEXT("Current Angle to target: %f"), CurrentAngle));
						if (CurrentAngle < BestAngle)
						{
							BestAngle = CurrentAngle;
							BestActor = Actor;
						}
					}
				}
			}
		}
		if (BestActor)
		{
			PlayerCharacter->SetCombatCameraTarget(BestActor);
			if (const ABaseManaEnemy* BaseEnemy = Cast<ABaseManaEnemy>(BestActor)) BaseEnemy->SetTargetWidgetIcon(true);
			const FGameplayTag FocusTag = FGameplayTag::RequestGameplayTag("Player.IsFocused");
			AbilitySystemComponent->AddLooseGameplayTag(FocusTag);
			//GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Orange, FString::Printf(TEXT("Best Actor found: %s"), *BestActor->GetName()));
		}
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}
