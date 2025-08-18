// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/RightHandEquipment.h"

ARightHandEquipment::ARightHandEquipment()
{
}

void ARightHandEquipment::SetItem()
{
	//DON'T Call super on this we don't want it. That will set a static mesh when weapons need skeletal ones :)
	const FWeaponItemData*  WeaponItemData = nullptr;

	if (ItemData.ItemID.DataTable)
	{
		static const FString ContextString(TEXT("Finding row in Item Data Table"));
		if (const FPickUpItems* ItemRow = ItemData.ItemID.DataTable->FindRow<FPickUpItems>(ItemData.ItemID.RowName, ContextString))
		{
			static const FString WeaponContextString(TEXT("Finding row in Weapon Data Table"));
			WeaponItemData = ItemRow->ItemTypeData.DataTable->FindRow<FWeaponItemData>(ItemRow->ItemTypeData.RowName, ContextString);
		}
	}

	if (WeaponItemData)
	{
		SkeletalMesh->SetSkeletalMesh(WeaponItemData->SkeletalMesh);
		EquipmentType = WeaponItemData->EquipmentType;
		Damage = WeaponItemData->Damage;
		EquipMontage = WeaponItemData->EquipMontage;
		AttackCombo = WeaponItemData->AttackCombo;
		AttackComboMovement = WeaponItemData->AttackComboMovement;
		EquipAttack = WeaponItemData->EquipAttack;
		EquipAttackMovement = WeaponItemData->EquipAttackMovement;
		EquipmentTypeClass = WeaponItemData->EquipmentTypeClass;
		EquipmentSocket = WeaponItemData->EquipmentSocket;
	}
}
