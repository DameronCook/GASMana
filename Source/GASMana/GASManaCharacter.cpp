// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASManaCharacter.h"
#include "Engine/LocalPlayer.h"
#include "AbilitySystemComponent.h"
#include "Public/ManaAttributeSet.h"
#include "Components/CapsuleComponent.h"
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

	Attributes = CreateDefaultSubobject<UManaAttributeSet>(TEXT("Attributes"));

}

void AGASManaCharacter::AddEquipment(FName SocketName, TSubclassOf<AManaEquipmentParent> EquipmentClass)
{
	UWorld* World = GetWorld();
	USkeletalMeshComponent* SkeletalMesh = GetMesh();

	if (World && SkeletalMesh) {
		AManaEquipmentParent* Equipment = World->SpawnActor<AManaEquipmentParent>(EquipmentClass, SkeletalMesh->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_World));
		Equipment->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
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
	/*
	if (GEngine && AbilitySystemComponent)
	{
		int32 Key = 1; // Unique key for each message
		for (const FGameplayAbilitySpec& Spec : AbilitySystemComponent->GetActivatableAbilities())
		{
			if (Spec.Ability)
			{
				FString AbilityName = Spec.Ability->GetClass()->GetName();
				GEngine->AddOnScreenDebugMessage(Key++, 5.0f, FColor::Yellow, FString::Printf(TEXT("Granted Ability: %s"), *AbilityName));

				// Optionally, show asset tags
				GEngine->AddOnScreenDebugMessage(Key++, 5.0f, FColor::Cyan, FString::Printf(TEXT("Ability Asset Tags: %s"), *Spec.Ability->GetAssetTags().ToString()));
			}
		}
	}
	*/
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