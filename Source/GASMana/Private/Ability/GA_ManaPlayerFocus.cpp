// Fill out your copyright notice in the Description page of Project Settings.


#include "Ability/GA_ManaPlayerFocus.h"

#include "PlayerManaCharacter.h"
#include "Actors/BaseManaEnemy.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"


UGA_ManaPlayerFocus::UGA_ManaPlayerFocus()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	FGameplayTagContainer Tags;
	Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.Action.Focus")));
	SetAssetTags(Tags);

	//Blocked Tags
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsFocused")));
}

TArray<class AActor*> UGA_ManaPlayerFocus::FindAllActorsInRange(const APlayerManaCharacter* PlayerCharacter,
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

bool UGA_ManaPlayerFocus::DrawLineToTarget(const ACharacter* Character, const AActor* OverlappedActor) const
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

float UGA_ManaPlayerFocus::CalculateAngleToTarget(ACharacter* Character, const AActor* OverlappedActor)
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

void UGA_ManaPlayerFocus::ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                          FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	if (UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
	{
		if (APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get()))
		{
			PlayerCharacter->SetFocusAbility(this);
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
							//GEngine->AddOnScreenDebugMessage(4, 5.f, FColor::Orange, FString::Printf(TEXT("Current Angle to target: %f"), CurrentAngle));
							if (const float CurrentAngle = CalculateAngleToTarget(PlayerCharacter, Actor); CurrentAngle < BestAngle)
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
				AbilitySystemComponent->ApplyGameplayEffectToSelf(
			PlayerCharacter->GetFocusEffectClass()->GetDefaultObject<UGameplayEffect>(),
			1.0f,
			AbilitySystemComponent->MakeEffectContext()
		);
				//GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Orange, FString::Printf(TEXT("Best Actor found: %s"), *BestActor->GetName()));
			}
		}
	}
}

void UGA_ManaPlayerFocus::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "EndAbility called!");
	if (APlayerManaCharacter* PlayerChar = Cast<APlayerManaCharacter>(ActorInfo->AvatarActor.Get()))
	{
		PlayerChar->SetFocusAbility(nullptr);

		if (UAbilitySystemComponent* AbilitySystemComponent = ActorInfo->AbilitySystemComponent.Get())
		{
			AbilitySystemComponent->RemoveActiveGameplayEffectBySourceEffect(PlayerChar->GetFocusEffectClass(), AbilitySystemComponent);
		}
		
		if (AActor* Target = PlayerChar->GetCombatCameraTarget())
		{
			if (const ABaseManaEnemy* CombatTarget = Cast<ABaseManaEnemy>(Target))
			{
				CombatTarget->SetTargetWidgetIcon(false);
			}
			PlayerChar->SetCombatCameraTarget(nullptr);
		}
	}
}

void UGA_ManaPlayerFocus::EndFocusAbility()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}
