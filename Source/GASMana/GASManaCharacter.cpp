// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASManaCharacter.h"
#include "AbilitySystemComponent.h"
#include "PlayerManaCharacter.h"
#include "Actors/BaseManaEnemy.h"
#include "AI/AIC_NPC.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/LeftHandEquipment.h"
#include "Item/RightHandEquipment.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "Public/ManaAttributeSet.h"
#include "Public/Interface/I_PickUpInterface.h"

//DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AGASManaCharacter


AGASManaCharacter::AGASManaCharacter()
{
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AGASManaCharacter::OnCapsuleBeginOverlap);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel4, ECollisionResponse::ECR_Block);
	
	Attributes = CreateDefaultSubobject<UManaAttributeSet>(TEXT("Attributes"));
}

void AGASManaCharacter::SetEquipment(AEquipment* Equipment)
{
	if (Equipment)
	{
		Equipment->SetPickedUp(true);

		const FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Item.NotPickedUp");
		Equipment->GetTagContainer().RemoveTag(Tag);
		
		EquipmentState = Equipment->GetEquipmentType();

		switch (Equipment->GetItemType())
		{
			case EItemType::EIT_LeftHandedEquipment:
				SetEquipMontageLeft(Equipment->GetEquipMontage());
				break;
			case EItemType::EIT_RightHandedEquipment:
				SetEquipMontageRight(Equipment->GetEquipMontage());
				break;
			default:
				break;
		}

		Equipment->DisablePickUpCollision();
		
		const FName& EquipSocket = Equipment->GetEquipmentSocket(); 
		EquipGearToSocket(Equipment, EquipSocket);
		
		if (Equipment->GetEquipTypeClass())
		{
			GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(
				Equipment->GetEquipTypeClass()->GetDefaultObject<UGameplayEffect>(), 1.0f,
				GetAbilitySystemComponent()->MakeEffectContext());
		}
	}
}

void AGASManaCharacter::AttachWeaponToBack() const
{

	if (RightHandEquipment) EquipGearToSocket(RightHandEquipment, "RightHandEquipSocket");
	if (LeftHandEquipment) EquipGearToSocket(LeftHandEquipment, "LeftHandEquipSocket");

	RemoveAnyEquipClass();
}

void AGASManaCharacter::AttachWeaponToHand()
{
	if (RightHandEquipment) SetEquipment(RightHandEquipment);
	if (LeftHandEquipment) SetEquipment(LeftHandEquipment);
}

void AGASManaCharacter::EquipGearToSocket(const AEquipment* GearToEquip, const FName SocketName) const
{
	if (GearToEquip)
	{
		GearToEquip->AttachMeshToSocket(GetMesh(), SocketName);
	}
}

void AGASManaCharacter::RemoveAnyEquipClass() const
{
	if (!AbilitySystemComponent) return;

	// Create a tag container with the parent tag
	FGameplayTagContainer EquipTags;
	EquipTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.Equipped")));

	// Remove all active effects that granted any tag under Character.Equipped.*
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(EquipTags);

}

bool AGASManaCharacter::GotMovementInput() const
{
	return false;
}

void AGASManaCharacter::SetOverlappingItem(class AItem* Item)
{
	OverlappingItem = Item;
}

void AGASManaCharacter::GrabOverlappingItem()
{
	if (OverlappingItem)
	{
		if (AEquipment* Equipment = Cast<AEquipment>(OverlappingItem))
		{
			Equipment->GetStimuliSource()->UnregisterFromSense(UAISense_Sight::StaticClass());
			ALeftHandEquipment* LEquipment;
			ARightHandEquipment* REquipment;
			switch (Equipment->GetItemType())
			{
			case EItemType::EIT_RightHandedEquipment:
				REquipment = Cast<ARightHandEquipment>(Equipment);
				//TODO: Make sure to implement a drop equipment feature
				if (RightHandEquipment) RightHandEquipment->Destroy();
				if (REquipment) RightHandEquipment = REquipment;
				break;
			case EItemType::EIT_LeftHandedEquipment:
				LEquipment = Cast<ALeftHandEquipment>(Equipment);
				//TODO: Make sure to implement a drop equipment feature
				if (LeftHandEquipment) LeftHandEquipment->Destroy();
				LeftHandEquipment = LEquipment;
				break;
			default:
				break;
			}
			SetEquipment(Equipment);
			OverlappingItem = nullptr;
			PlayAnimMontage(GetPickUpMontage());
		}
	}
}

void AGASManaCharacter::Blocking()
{
	//Empty for now. Whenever other actors inherit from this, they can override this function
}

void AGASManaCharacter::FinishedBlocking()
{
	//Empty for now. Whenever other actors inherit from this, they can override this function

}

void AGASManaCharacter::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                              const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->Implements<UI_PickUpInterface>())
	{
		//UE_LOG(LogTemp, Warning, TEXT("OtherActor: %s"), *GetNameSafe(OtherActor));
		UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();
		II_PickUpInterface::Execute_OnPickedUp(OtherActor, this, AbilitySystem);
	}
}

bool AGASManaCharacter::Attack()
{
	if (RightHandEquipment)
	{
		GetMontageToPlay();
		const FGameplayTagContainer AttackType = GetAttackType(); 
		return GetAbilitySystemComponent()->TryActivateAbilitiesByTag(AttackType, true);
	}
	return true;
}

void AGASManaCharacter::DirectionalHitReact(const FVector& HitterLocation, bool IsFinisher)
{
	if (IsFinisher)
	{
		SetHitReactMontage(MovingReactMontage);
	}
	else
	{
		SetHitReactMontage(StationaryReactMontage);
	}

	PlayFlashEffect(HitFlashColor, 0.5f);
	
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(HitterLocation.X, HitterLocation.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
	
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	Theta = FMath::RadiansToDegrees(Theta);

	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	HitReactSection = FName("FromBack");

	if (Theta >= -45.f && Theta < 45.f)
	{
		HitReactSection = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		HitReactSection = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		HitReactSection = FName("FromRight");
	}
	
	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(HitReactTagContainer, true);
}

FGameplayTagContainer AGASManaCharacter::GetAttackType() const
{
	return AttackTagContainer;
}

void AGASManaCharacter::GetMontageToPlay()
{
	if (UAnimMontage* MontageToPlay = RightHandEquipment->GetAttackCombo()) SetAttackMontage(MontageToPlay);
}

void AGASManaCharacter::PlayerCharacterDieMontage(const FVector& HitLocation)
{
	if (bIsDead) return;

	bIsDead = true;
	
	PlayFlashEffect(HitFlashColor, 0.5f);

	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(HitLocation.X, HitLocation.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
	
	const double CosTheta = FVector::DotProduct(Forward, ToHit);
	double Theta = FMath::Acos(CosTheta);
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	Theta = FMath::RadiansToDegrees(Theta);

	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	DeathReactSection = FName("FromBack");
	DeathType = EDeathType::Forward;

	if (Theta >= -45.f && Theta < 45.f)
	{
		DeathReactSection = FName("FromFront");
		DeathType = EDeathType::Backward;
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		DeathReactSection = FName("FromLeft");
		DeathType = EDeathType::Right;
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		DeathReactSection = FName("FromRight");
		DeathType = EDeathType::Left;
	}
}

void AGASManaCharacter::PlayFlashEffect(FVector InColor, float FlashLength) const
{
	if (USkeletalMeshComponent* CharMesh = GetMesh())
	{
		CharMesh->SetVectorParameterValueOnMaterials("EffectColor", InColor);
		CharMesh->SetScalarParameterValueOnMaterials("StartTime",
			GetWorld()->GetTimeSeconds());
		CharMesh->SetScalarParameterValueOnMaterials("EffectLength", FlashLength);
	}
}

void AGASManaCharacter::MeleeAttackNotify(FVector AttackPosition, bool IsFinisher = false)
{
	//->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Melee Attack Read");
	UWorld* World = GetWorld();
	float Radius = 225.f;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	UClass* Class = StaticClass();
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);

	if (TArray<AActor*> OutActors; UKismetSystemLibrary::SphereOverlapActors(World, AttackPosition, Radius, ObjectTypes, Class, IgnoreActors, OutActors))
	{
		for (AActor* HitActor : OutActors)
		{
			if (AGASManaCharacter* HitManaCharacter = Cast<AGASManaCharacter>(HitActor))
			{
				if (!HitManaCharacter->bIsDead)
				{
					if (UAbilitySystemComponent* ASC = HitManaCharacter->GetAbilitySystemComponent())
					{
						//First, check if the hit mana character is blocking
						if (ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.IsBlocking"))))
						{
							//Now check if the hit character is facing the attacked character
							const FVector Forward = HitActor->GetActorForwardVector();
							const FVector ImpactLowered(HitActor->GetActorLocation().X, HitActor->GetActorLocation().Y, GetActorLocation().Z);
							const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal();
							const double CosTheta = FVector::DotProduct(Forward, ToHit);
							if (CosTheta <= 0)
							{
								//If we hit a blocking player, do a little screenshake
								if (APlayerManaCharacter* PChar = Cast<APlayerManaCharacter>(HitManaCharacter))
								{
									PChar->ActivateCamShake(HitPlayerShieldCameraShake);
								}
								//If we're a player who hit a blocking enemy, then let them know we're vulnerable
								else if (Cast<APlayerManaCharacter>(this))
								{
									if (ABaseManaEnemy* EChar = Cast<ABaseManaEnemy>(HitManaCharacter))
									{
										EChar->GetEnemyController()->GetBlackboardComponent()->SetValueAsBool("PlayerIsVulnerable", true);
									}
								}
								
								//If they are, then THE ATTACKING character must activate their hit stun ability
								this->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(ShieldStunTagContainer, true);
								return;	
							}
						}

						//If our hit mana character is rolling don't apply anything below
						if (!ASC->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Player.IsRolling")))
						{
							ASC->ApplyGameplayEffectToSelf(HitManaCharacter->GetDamageEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
							
							//After applying damager, if we're alive apply knockback
							if (HitManaCharacter->IsAlive())
							{
								HitManaCharacter->StartHitStop(0.1f, HitManaCharacter);
								StartHitStop(0.1f, this);
								if (ABaseManaEnemy* HitEnemyCharacter = Cast<ABaseManaEnemy>(HitManaCharacter))
								{
									HitEnemyCharacter->GetEnemyController()->GetBlackboardComponent()->SetValueAsObject("TargetToFollow", this);
									HitEnemyCharacter->GetEnemyController()->GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", true);
								}
								HitManaCharacter->DirectionalHitReact(GetActorLocation(), IsFinisher);
							}
							//If we're dead, die
							else
							{
								HitManaCharacter->Die(GetActorLocation());

								//If the attacker is a player, clear the combat camera target.
								if (APlayerManaCharacter* PlayerChar = Cast<APlayerManaCharacter>(this))
								{
									PlayerChar->SetCombatCameraTarget(nullptr);
								}
							}
						}
					}
				}
			}
		}
	}
}

void AGASManaCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	InitializeAttributes();
	GiveDefaultAbilities();
}

void AGASManaCharacter::GiveDefaultAbilities()
{
	if (HasAuthority() && AbilitySystemComponent)
	{
		for (TSubclassOf<UGameplayAbility>& StartUpAbility : DefaultAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartUpAbility.GetDefaultObject(), 1,
				0));
		}
	}

	/* 	Uncomment below to see what abilities and specs the player is given
	
	if (GEngine && AbilitySystemComponent)
	{
		int32 Key = 1;
		for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if (Spec.Ability)
			{
				FString AbilityName = Spec.Ability->GetClass()->GetName();
				GEngine->AddOnScreenDebugMessage(Key++, 5.0f, FColor::Yellow, FString::Printf(TEXT("Granted Ability: %s"), *AbilityName));

				GEngine->AddOnScreenDebugMessage(Key++, 5.0f, FColor::Cyan, FString::Printf(TEXT("Ability Asset Tags: %s"), *Spec.Ability->GetAssetTags().ToString()));
			}
		}
	}
	*/
}

void AGASManaCharacter::EquipLeftHandGear()
{
	if (LeftHandEquipment)
	{
		SetEquipment(LeftHandEquipment);
	}
}

void AGASManaCharacter::EquipRightHandGear()
{
	if (RightHandEquipment)
	{
		SetEquipment(RightHandEquipment);
	}
}

void AGASManaCharacter::InitializeAttributes()
{
	if (AbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		const FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			DefaultAttributeEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle =
				AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void AGASManaCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	InitializeAttributes();
}

void AGASManaCharacter::InstantlyUnequipGear()
{
	EquipmentState = EEquipmentState::EES_Unequipped;
	AttachWeaponToBack();
}

void AGASManaCharacter::SetDefaultCombos()
{
	ComboAttackName = DefaultComboName;
	bIsAttackWindowOpen = false;
}

void AGASManaCharacter::SetNextComboSegment(const FName NextCombo)
{
	ComboAttackName = NextCombo;
	bIsAttackWindowOpen = true;
}

bool AGASManaCharacter::IsAlive() 
{
	float hp = GetAbilitySystemComponent()->GetNumericAttribute(UManaAttributeSet::GetHealthAttribute());
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Health: %f"), hp));
	if (Attributes->GetHealth() < 0.f)
	{
		return false;
	}

	return true;
}

void AGASManaCharacter::Die(const FVector& HitLocation)
{
	//Nothing for now let this be overwritten
}

void AGASManaCharacter::Ragdoll()
{
	if (UCharacterMovementComponent* CharacterComp = Cast<UCharacterMovementComponent>(GetMovementComponent()))
	{
		CharacterComp->StopMovementImmediately();
		CharacterComp->DisableMovement();
		CharacterComp->SetComponentTickEnabled(false);
	}

	USkeletalMeshComponent* MeshComp = GetMesh();
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (!MeshComp || !MeshComp->GetPhysicsAsset())
	{
		return;
	}

	MeshComp->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);

	if (UAnimInstance* AnimInst = MeshComp->GetAnimInstance())
	{
		AnimInst->StopAllMontages(0.0f);
	}

	MeshComp->SetCollisionProfileName(TEXT("Ragdoll"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetAllBodiesSimulatePhysics(true);
	MeshComp->SetSimulatePhysics(true);
	MeshComp->WakeAllRigidBodies();
	MeshComp->bBlendPhysics = true;
	MeshComp->RecreatePhysicsState();
	MeshComp->MarkRenderStateDirty();
	MeshComp->SetVisibility(true, true);

	if (Capsule)
	{
		// Keep collision enabled but ignore Pawn and Camera so players / projectiles don't block
		Capsule->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		Capsule->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
		Capsule->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
		Capsule->SetCollisionResponseToChannel(ECC_Visibility, ECR_Ignore);
	}

	UE_LOG(LogTemp, Warning, TEXT("Mesh parent: %s, Hidden:%d, Simulating:%d"), *GetNameSafe(MeshComp->GetAttachParent()), MeshComp->bHiddenInGame, MeshComp->IsSimulatingPhysics());
}

void AGASManaCharacter::RemoveFreeTag() const
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, "RemoveFreeTag called!");
	const FGameplayTag FreeTag = FGameplayTag::RequestGameplayTag(FName("Character.IsFree"));
	GetAbilitySystemComponent()->RemoveLooseGameplayTag(FreeTag);
}

void AGASManaCharacter::LoadMe()
{
	//Empty for now, designed to be overridden	
}

void AGASManaCharacter::UnloadMe()
{
	//Empty for now, designed to be overridden	
}
