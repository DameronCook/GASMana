// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASManaCharacter.h"
#include "Engine/LocalPlayer.h"
#include "AbilitySystemComponent.h"
#include "Public/ManaAttributeSet.h"
#include "Components/CapsuleComponent.h"
#include "Item/LeftHandEquipment.h"
#include "Public/Interface/I_PickUpInterface.h"
#include "Item/RightHandEquipment.h"

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

	Attributes = CreateDefaultSubobject<UManaAttributeSet>(TEXT("Attributes"));

}

void AGASManaCharacter::SetEquipment(AEquipment* Equipment)
{
	if (Equipment)
	{
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
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(StartUpAbility.GetDefaultObject(), 1,
				0));
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

void AGASManaCharacter::EquipLeftHandGear()
{
	if (LeftHandEquipment)
	{
		EquipGearToSocket(LeftHandEquipment, "hand_lSocket");
		SetEquipment(LeftHandEquipment);
	}
}

void AGASManaCharacter::EquipRightHandGear()
{
	if (RightHandEquipment)
	{
		EquipGearToSocket(RightHandEquipment, "hand_rSocket");
		SetEquipment(RightHandEquipment);
	}
}

void AGASManaCharacter::InitializeAttributes()
{
	if (AbilitySystemComponent && DefaultAttributeEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);
		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
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
