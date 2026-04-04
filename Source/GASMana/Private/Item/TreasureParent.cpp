// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/TreasureParent.h"
#include "Item/Item.h"
#include "PlayerManaCharacter.h"
#include "Item/DT_TreasurePickups.h"

ATreasure::ATreasure()
{
	// TMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GhostMesh0"));
	// TMeshComponent->SetupAttachment(RootComponent);
	// //TMeshComponent->SetStaticMesh(ConstructorStatics.TestBuildin_Finel.Get());
}

void ATreasure::BeginPlay()
{
	Super::BeginPlay();
	SetItem();
	
}

void ATreasure::SetItem()
{
	Super::SetItem();
	const FTreasureDT*  TreasureData = nullptr;

	if (ItemData.ItemID.DataTable)
	{
		static const FString ContextString(TEXT("Finding row in Item Data Table"));
		if (const FPickUpItems* ItemRow = ItemData.ItemID.DataTable->FindRow<FPickUpItems>(ItemData.ItemID.RowName, ContextString))
		{
			static const FString TreasureContextString(TEXT("Finding row in Treasure Data Table"));
			TreasureData = ItemRow->ItemTypeData.DataTable->FindRow<FTreasureDT>(ItemRow->ItemTypeData.RowName, ContextString);
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, FString::Printf(TEXT("Money Count: %f"), TreasureData->TAmount));
		}
	}
	if (TreasureData)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, FString::Printf(TEXT("Money Count: %f"), TreasureAmount));
		TreasureAmount = TreasureData->TAmount;
	}
}

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)/
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (APlayerManaCharacter* Char = Cast<APlayerManaCharacter>(OtherActor))
	{
		if (Char->IsAlive())
		{
			//Char->TotalMoneyCounter += TreasureAmount;
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, FString::Printf(TEXT("Money Count: %f"), Char->TotalMoneyCounter));
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Setting Treasure Amount");
			//UAbilitySystemComponent* AbilitySystem = Char->GetAbilitySystemComponent();
			/*if (AbilitySystem && PickUpEffect)
			{
				FGameplayEffectContextHandle EffectContext = AbilitySystem->MakeEffectContext();
				EffectContext.AddSourceObject(this);

				//FGameplayEffectSpecHandle SpecHandle = AbilitySystem->MakeOutgoingSpec(PickUpEffect, 1.0f, EffectContext);
				if (SpecHandle.IsValid())
				{
					AbilitySystem->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}*/

			/*if (!FlashColor.IsNearlyZero())
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Playing Material Effect");
				Char->PlayFlashEffect(FlashColor, FlashLength);
			}*/

			/*if (TMesh)
			{
				TMesh->Deactivate();
			}*/
		
			Destroy();
		}
	}
}	