// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Equipment.h"
#include "LeftHandEquipment.generated.h"

class UGameplayEffect;
/**
 * 
 */
USTRUCT(BlueprintType)
struct FLeftHandItemData : public FTableRowBase
{
	GENERATED_BODY()

	/** Type of animation state the character should switch to based on this weapon **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EEquipmentState EquipmentType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Durability;
	
	/** Montage for equipping **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* EquipMontage;

	/** Skeletal Mesh **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* SkeletalMesh;

	/** Gameplay Effect put on the player that informs their equip state */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> EquipmentTypeClass;
	
	/** Name of Socket that the Equipment should attach to **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName EquipmentSocket;
};

UCLASS()
class GASMANA_API ALeftHandEquipment : public AEquipment
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	int Durability;

public:
	// Sets default values for this actor's properties
	ALeftHandEquipment();
	
	virtual void SetItem() override;

	FORCEINLINE int GetDurability() const { return Durability; }
};
