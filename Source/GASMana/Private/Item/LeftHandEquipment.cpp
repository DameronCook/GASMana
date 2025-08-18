// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/LeftHandEquipment.h"


// Sets default values
ALeftHandEquipment::ALeftHandEquipment()
{
}

/* Sets the Item */
void ALeftHandEquipment::SetItem()
{
	//DON'T Call super on this we don't want it. That will set a static mesh when weapons need skeletal ones :)
	const FLeftHandItemData*  LeftHandEquippedItemData = nullptr;

	if (ItemData.ItemID.DataTable)
	{
		static const FString ContextString(TEXT("Finding row in Item Data Table"));
		if (const FPickUpItems* ItemRow = ItemData.ItemID.DataTable->FindRow<FPickUpItems>(ItemData.ItemID.RowName, ContextString))
		{
			static const FString WeaponContextString(TEXT("Finding row in Weapon Data Table"));
			LeftHandEquippedItemData = ItemRow->ItemTypeData.DataTable->FindRow<FLeftHandItemData>(ItemRow->ItemTypeData.RowName, ContextString);
		}
	}

	if (LeftHandEquippedItemData)
	{
		SkeletalMesh->SetSkeletalMesh(LeftHandEquippedItemData->SkeletalMesh);
		EquipmentType = LeftHandEquippedItemData->EquipmentType;
		Durability = LeftHandEquippedItemData->Durability;
		EquipMontage = LeftHandEquippedItemData->EquipMontage;
		EquipmentTypeClass = LeftHandEquippedItemData->EquipmentTypeClass;
		EquipmentSocket = LeftHandEquippedItemData->EquipmentSocket;
	}
}