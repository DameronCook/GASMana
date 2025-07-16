// Fill out your copyright notice in the Description page of Project Settings.


#include "PickUps/PickUp_Parent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "../GASManaCharacter.h"
#include "Components/CapsuleComponent.h"


// Sets default values
APickUp_Parent::APickUp_Parent()
{
	PrimaryActorTick.bCanEverTick = false;

	CapsuleComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleComponent->InitCapsuleSize(30.0f, 30.0f);
	CapsuleComponent->SetupAttachment(RootComponent);

	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CapsuleComponent->SetCollisionObjectType(ECC_WorldDynamic);
	CapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CapsuleComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void APickUp_Parent::OnPickedUp_Implementation(AActor* Picker, UAbilitySystemComponent* AbilitySystem)
{
	if (AbilitySystem && PickUpEffect)
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystem->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(PickUpEffect, 1.0f, EffectContext);
		if (SpecHandle.IsValid())
		{
			AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}

	if (!FlashColor.IsNearlyZero())
	{
		if (AGASManaCharacter* Char = Cast<AGASManaCharacter>(Picker))
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Playing Material Effect");
			Char->PlayFlashEffect(FlashColor, FlashLength);
		}
	}

	Destroy();
}

