// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/ManaPickUp.h"

#include "GameplayEffectTypes.h"
#include "GASMana/GASManaCharacter.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h" 

AManaPickUp::AManaPickUp()
{
}

void AManaPickUp::BeginPlay()
{
	Super::BeginPlay();

	ManaComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ManaEffect, GetActorLocation(), FRotator::ZeroRotator);
	ManaComponent->Activate();
}

void AManaPickUp::SetItem()
{
	//Super::SetItem();
	//DON'T Call super on this we don't want it. That will set a static mesh when weapons need skeletal ones :)
	const FManaData*  ManaItemData = nullptr;

	if (ItemData.ItemID.DataTable)
	{
		static const FString ContextString(TEXT("Finding row in Item Data Table"));
		if (const FPickUpItems* ItemRow = ItemData.ItemID.DataTable->FindRow<FPickUpItems>(ItemData.ItemID.RowName, ContextString))
		{
			static const FString WeaponContextString(TEXT("Finding row in Weapon Data Table"));
			ManaItemData = ItemRow->ItemTypeData.DataTable->FindRow<FManaData>(ItemRow->ItemTypeData.RowName, ContextString);
		}
	}

	if (ManaItemData)
	{
		PickUpEffect = ManaItemData->PickUpEffect;
		ManaEffect = ManaItemData->ManaEffect;
		FlashColor = ManaItemData->FlashColor;
		FlashLength = ManaItemData->FlashLength;
		FlashLengthName = ManaItemData->FlashLengthName;
		FlashParamName = ManaItemData->FlashParamName;
		if (!ManaComponent)
		{
			ManaComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), ManaEffect, GetActorLocation(), FRotator::ZeroRotator);
			ManaComponent->Activate();
		}
	}
}

void AManaPickUp::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (const AGASManaCharacter* Char = Cast<AGASManaCharacter>(OtherActor))
	{
		UAbilitySystemComponent* AbilitySystem = Char->GetAbilitySystemComponent();
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
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Playing Material Effect");
			Char->PlayFlashEffect(FlashColor, FlashLength);
		}

		ManaComponent->Deactivate();
		Destroy();
	}
}

