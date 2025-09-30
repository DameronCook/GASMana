// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASManaCharacter.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Item/LeftHandEquipment.h"
#include "Item/RightHandEquipment.h"
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
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Setting Equip Type Class");
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
				if (REquipment) RightHandEquipment = REquipment;
				break;
			case EItemType::EIT_LeftHandedEquipment:
				LEquipment = Cast<ALeftHandEquipment>(Equipment);
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



FGameplayTagContainer AGASManaCharacter::GetAttackType() const
{
	return AttackTagContainer;
}

void AGASManaCharacter::GetMontageToPlay()
{
	if (UAnimMontage* MontageToPlay = RightHandEquipment->GetAttackCombo()) SetAttackMontage(MontageToPlay);
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
	return true;
}

void AGASManaCharacter::RemoveFreeTag() const
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, "RemoveFreeTag called!");
	const FGameplayTag FreeTag = FGameplayTag::RequestGameplayTag(FName("Character.IsFree"));
	GetAbilitySystemComponent()->RemoveLooseGameplayTag(FreeTag);
}
