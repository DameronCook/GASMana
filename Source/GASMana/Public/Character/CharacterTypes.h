#pragma once

UENUM(BlueprintType)
enum class EEquipmentState : uint8
{
	EES_Unequipped UMETA(DisplayName = "Unequipped"),
	EES_EquippedOneHandedWeapon UMETA(DisplayName = "Equipped One-Handed Weapon"),
	EES_EquippedTwoHandedWeapon UMETA(DisplayName = "Equipped Two-Handed Weapon"),
	EES_EquippedBow UMETA(DisplayName = "Equipped Bow")
};