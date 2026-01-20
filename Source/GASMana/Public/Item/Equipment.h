// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Item.h"
#include "Equipment.generated.h"

class UAIPerceptionStimuliSourceComponent;
class UGameplayEffect;

UCLASS()
class GASMANA_API AEquipment : public AItem
{
	GENERATED_BODY()
protected:
	
	/** Skeletal mesh of the equipment **/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;
	
	/** Type of animation state the character should switch to based on this weapon **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	EEquipmentState EquipmentType;
	
	/** Montage for equipping **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontage;

	/** Gameplay Effect put on the player that informs their equip state */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> EquipmentTypeClass;
	
	/** Name of Socket that the Equipment should attach to **/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	FName EquipmentSocket;

	/** Tag to be granted to the character when an item is equipped. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	FGameplayTag EquipmentTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipment")
	UAIPerceptionStimuliSourceComponent* StimuliSource;

	UPROPERTY(BlueprintReadOnly)
	int CurDurability;

public:
	AEquipment();

	virtual void BeginPlay() override;

	void AttachMeshToSocket(USceneComponent* InParent, FName InSocketName) const;

	virtual void BreakEquipment();

	FORCEINLINE EEquipmentState GetEquipmentType() const { return EquipmentType; }
	FORCEINLINE UAnimMontage* GetEquipMontage() const { return EquipMontage; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetEquipTypeClass() const { return EquipmentTypeClass; }
	FORCEINLINE FName GetEquipmentSocket() const { return EquipmentSocket; }
	FORCEINLINE UAIPerceptionStimuliSourceComponent* GetStimuliSource() const { return StimuliSource; }
	FORCEINLINE int GetCurDurability() const { return CurDurability; }
	void SetCurDurability(const int NewDurability);
};
