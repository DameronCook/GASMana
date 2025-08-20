// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "RightHandEquipment.generated.h"

class UGameplayEffect;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FWeaponItemData : public FTableRowBase
{
	GENERATED_BODY()

	/** Type of animation state the character should switch to based on this weapon **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipmentState EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Damage;
	
	/** Montage for equipping **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* EquipMontage;
	
	/** Montage for Attack Combos **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* AttackCombo;

	/** Montage for Attack Combos with movement **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* AttackComboMovement;

	/** Montage for an Equip attack **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* EquipAttack;

	/** Montage for an Equip Attack with movement **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* EquipAttackMovement;

	/** Skeletal Mesh **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* SkeletalMesh;

	/** Gameplay Effect put on the player that informs their equip state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> EquipmentTypeClass;
	
	/** Name of Socket that the Equipment should attach to **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EquipmentSocket;
	
	/** Name of Socket that the Equipment should attach to **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* AirAttackMontage;
	
};

UCLASS()
class GASMANA_API ARightHandEquipment : public AEquipment
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	int Damage;
	
	/** Montage for Attack Combos **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackCombo;

	/** Montage for Attack Combos with movement **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackComboMovement;

	/** Montage for an Equip attack **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipAttack;

	/** Montage for an Equip Attack with movement **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment | Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipAttackMovement;

public:
	ARightHandEquipment();

	virtual void SetItem() override;

	FORCEINLINE int GetDamage() const { return Damage; }
	FORCEINLINE UAnimMontage* GetAttackCombo() const { return AttackCombo; }
	FORCEINLINE UAnimMontage* GetAttackComboMovement() const { return AttackComboMovement; }
	FORCEINLINE UAnimMontage* GetEquipAttack() const { return EquipAttack; }
	FORCEINLINE UAnimMontage* GetEquipAttackMovement() const { return EquipAttackMovement; }
};
