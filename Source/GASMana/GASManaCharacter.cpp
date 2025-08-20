// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASManaCharacter.h"
#include "Engine/LocalPlayer.h"
#include "AbilitySystemComponent.h"
#include "Public/ManaAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Public/Interface/I_PickUpInterface.h"

//DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
/// AGASManaCharacter

AGASManaCharacter::AGASManaCharacter()
{
	GetCapsuleComponent()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AGASManaCharacter::OnCapsuleBeginOverlap);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	Attributes = CreateDefaultSubobject<UManaAttributeSet>(TEXT("Attributes"));

}

AManaEquipmentParent* AGASManaCharacter::AddEquipment(FName SocketName, TSubclassOf<AManaEquipmentParent> EquipmentClass)
{
	UWorld* World = GetWorld();
	USkeletalMeshComponent* SkeletalMesh = GetMesh();

	if (World && SkeletalMesh) {
		AManaEquipmentParent* Equipment = World->SpawnActor<AManaEquipmentParent>(EquipmentClass, SkeletalMesh->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_World));
		Equipment->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
		return Equipment;
	}
	return nullptr;
}

/* Equipping */
void AGASManaCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		if (EquipMontageRight)
		{
<<<<<<< Updated upstream
			AnimInstance->Montage_Play(EquipMontageRight);
			AnimInstance->Montage_JumpToSection(SectionName, EquipMontageRight);
		}

		if (EquipMontageLeft)
=======
			AbilitySystemComponent->TryActivateAbilitiesByTag(EquipTagContainer);
		}
		
		EquipmentState = Equipment->GetEquipmentType();

		FName EquippedTagName;

		switch (Equipment->GetItemType())
		{
			case EItemType::EIT_LeftHandedEquipment:
				SetEquipMontageLeft(Equipment->GetEquipMontage());
				EquippedTagName = "Character.Equipped.OneHanded.Left";
				break;
			case EItemType::EIT_RightHandedEquipment:
				SetEquipMontageRight(Equipment->GetEquipMontage());
				EquippedTagName = "Character.Equipped.OneHanded.Right";
				break;
			default:
				break;
		}

		Equipment->DisablePickUpCollision();

		if (!AbilitySystemComponent->ComponentHasTag(EquippedTagName) && Equipment->GetEquipTypeClass())
<<<<<<< Updated upstream
>>>>>>> Stashed changes
		{
			AnimInstance->Montage_Play(EquipMontageLeft);
			AnimInstance->Montage_JumpToSection(SectionName, EquipMontageLeft);
=======
		{
			GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(
				Equipment->GetEquipTypeClass()->GetDefaultObject<UGameplayEffect>(), 1.0f,
				GetAbilitySystemComponent()->MakeEffectContext());
>>>>>>> Stashed changes
		}
		
		const FName& EquipSocket = Equipment->GetEquipmentSocket(); 
		EquipGearToSocket(Equipment, EquipSocket);
		
	}
}

void AGASManaCharacter::SetEquipment()
{
	//Here we need to get the variables stored in the item and set the right equipment type up for the player. For now, let's just set it to a one handed weapon until we get other items in the game
	EquipmentState = EEquipmentState::EES_EquippedOneHandedWeapon;

	//so here we'd set the equip class to match the weapon type
	if (EquipmentTypeClass)
	{
		GetAbilitySystemComponent()->ApplyGameplayEffectToSelf(EquipmentTypeClass->GetDefaultObject<UGameplayEffect>(), 1.0f, GetAbilitySystemComponent()->MakeEffectContext());

	}
}

void AGASManaCharacter::AttatchWeaponToBack()
{

	EquipGearToSocket(RightHandEquipment, "RightHandEquipSocket");
	EquipGearToSocket(LeftHandEquipment, "LeftHandEquipSocket");

	RemoveAnyEquipClass();
}

void AGASManaCharacter::AttatchWeaponToHand()
{
<<<<<<< Updated upstream
<<<<<<< Updated upstream
	EquipGearToSocket(RightHandEquipment, "hand_rSocket");

	EquipGearToSocket(LeftHandEquipment, "hand_lSocket");

	SetEquipment();
=======
	if (RightHandEquipment) EquipRightHandGear();
	if (LeftHandEquipment) EquipLeftHandGear();
>>>>>>> Stashed changes
=======
	if (RightHandEquipment) EquipRightHandGear();
	if (LeftHandEquipment) EquipLeftHandGear();
>>>>>>> Stashed changes
}

void AGASManaCharacter::EquipGearToSocket(AManaEquipmentParent* GearToEquip, FName SocketName)
{
	if (GearToEquip)
	{
		GearToEquip->AttachMeshToSocket(GetMesh(), SocketName);
	}
}

void AGASManaCharacter::RemoveAnyEquipClass()
{
	if (!AbilitySystemComponent) return;

	// Create a tag container with the parent tag
	FGameplayTagContainer EquipTags;
	EquipTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Character.Equipped")));

	// Remove all active effects that granted any tag under Character.Equipped.*
	AbilitySystemComponent->RemoveActiveEffectsWithGrantedTags(EquipTags);

}

void AGASManaCharacter::Blocking()
{
	//Empty for now. Whenever other actors inherit from this, they can override this function
}

void AGASManaCharacter::FinishedBlocking()
{
	//Empty for now. Whenever other actors inherit from this, they can override this function

}

void AGASManaCharacter::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->Implements<UI_PickUpInterface>())
	{
		UE_LOG(LogTemp, Warning, TEXT("OtherActor: %s"), *GetNameSafe(OtherActor));
		UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();
		II_PickUpInterface::Execute_OnPickedUp(OtherActor, this, AbilitySystem);
	}
}

void AGASManaCharacter::PlayFlashEffect(FVector InColor, float FlashLength)
{
	USkeletalMeshComponent* CharMesh = GetMesh();

	if (CharMesh)
	{
		CharMesh->SetVectorParameterValueOnMaterials("EffectColor", InColor);
		CharMesh->SetScalarParameterValueOnMaterials("StartTime", GetWorld()->GetTimeSeconds());
		CharMesh->SetScalarParameterValueOnMaterials("EffectLength", FlashLength);
	}
}

void AGASManaCharacter::HandleMelee()
{
	//Empty for now. Whenever other actors inherit from this, they can override this function

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
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartUpAbility.GetDefaultObject(), 1, 0));
		}
	}

	//Uncomment below to see what abilities and specs the player is given
	//
	//if (GEngine && AbilitySystemComponent)
	//{
	//	int32 Key = 1; // Unique key for each message
	//	for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
	//	{
	//		if (Spec.Ability)
	//		{
	//			FString AbilityName = Spec.Ability->GetClass()->GetName();
	//			GEngine->AddOnScreenDebugMessage(Key++, 5.0f, FColor::Yellow, FString::Printf(TEXT("Granted Ability: %s"), *AbilityName));

	//			// Optionally, show asset tags
	//			GEngine->AddOnScreenDebugMessage(Key++, 5.0f, FColor::Cyan, FString::Printf(TEXT("Ability Asset Tags: %s"), *Spec.Ability->GetAssetTags().ToString()));
	//		}
	//	}
	//}
}

<<<<<<< Updated upstream
<<<<<<< Updated upstream
void AGASManaCharacter::EquipLeftHandGear()
=======
void AGASManaCharacter::EquipLeftHandGear() 
>>>>>>> Stashed changes
=======
void AGASManaCharacter::EquipLeftHandGear() 
>>>>>>> Stashed changes
{
	EquipGearToSocket(LeftHandEquipment, "hand_lSocket");
	SetEquipment(LeftHandEquipment);
}

void AGASManaCharacter::EquipRightHandGear() 
{
	EquipGearToSocket(RightHandEquipment, "hand_rSocket");
	SetEquipment(RightHandEquipment);
}

void AGASManaCharacter::InitializeAttributes()
{
	if (AbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);

		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
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
	AttatchWeaponToBack();
}

void AGASManaCharacter::SetDefaultCombos()
{
	ComboAttackName = DefaultComboName;
	bIsAttackWindowOpen = false;
}

void AGASManaCharacter::SetNextComboSegment(FName NextCombo)
{
	ComboAttackName = NextCombo;
	bIsAttackWindowOpen = true;
}
