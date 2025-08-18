// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include <GameplayEffectTypes.h>
#include "AbilitySystemInterface.h"
#include "Public/ManaAttributeSet.h"
#include "Logging/LogMacros.h"
#include "Character/CharacterTypes.h"
#include "Interface/ComboInterface.h"
#include "Item/Equipment.h"
#include "GASManaCharacter.generated.h"

class ALeftHandEquipment;
class ARightHandEquipment;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class AGASManaCharacter : public ACharacter, public IAbilitySystemInterface, public IComboInterface
{
	GENERATED_BODY()

	/* Ability Component*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	class UAbilitySystemComponent* AbilitySystemComponent;

	/* Hit React Montage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HitReactMontage;

	UPROPERTY()
	class UManaAttributeSet* Attributes;


	/** Damaged Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> DamageEffectClass;

public:
	AGASManaCharacter();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleMelee();

protected:
	UPROPERTY()
	ARightHandEquipment* RightHandEquipment;
	UPROPERTY()
	ALeftHandEquipment* LeftHandEquipment;

	/* Type of Equipment that the player is equipping*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> EquipmentTypeClass;

	UFUNCTION(BlueprintCallable, Category = "Blocking")
	virtual void Blocking();

	UFUNCTION(BlueprintCallable, Category = "Blocking")
	virtual void FinishedBlocking();

	/* Runs on begin overlap */
	UFUNCTION(BlueprintCallable, Category = "Overlap")
	virtual void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	//////////////////////////////////////

	UAnimMontage* EquipMontageRight;
	UAnimMontage* EquipMontageLeft;
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipment(const AEquipment* Equipment);

	void RemoveAnyEquipClass() const;

	const FName DefaultComboName = "Attack01";
	FName ComboAttackName = DefaultComboName;
	bool bIsAttackWindowOpen;


public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	void InstantlyUnequipGear();
	/* Instantly Equips Gear to left hand*/
	void EquipLeftHandGear() const;
	virtual void InitializeAttributes();
	virtual void GiveDefaultAbilities();
	
	UFUNCTION(BlueprintCallable)
	void PlayFlashEffect(FVector InColor, float FlashLength) const;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	/** Equip Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Equip", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> EquipClass;


	/** Equip Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer EquipTagContainer;

	//What Equipment the character has
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EEquipmentState EquipmentState;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void AttachWeaponToBack();
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void AttachWeaponToHand();

	void EquipGearToSocket(const AEquipment* GearToEquip, FName SocketName) const;

	FORCEINLINE class UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	FORCEINLINE ARightHandEquipment* GetRightHandEquipment() const { return RightHandEquipment; }
	FORCEINLINE ALeftHandEquipment* GetLeftHandEquipment() const { return LeftHandEquipment; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDamageEffectClass() const { return DamageEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetEquipEffectClass() const { return EquipClass; }
	FORCEINLINE UAnimMontage* GetHitReactMontage() const { return HitReactMontage; }
	FORCEINLINE UAnimMontage* GetEquipRightMontage() const { return EquipMontageRight; }
	FORCEINLINE UAnimMontage* GetEquipLeftMontage() const { return EquipMontageLeft; }
	FORCEINLINE EEquipmentState GetEquipmentState() const { return EquipmentState; }
	FORCEINLINE FGameplayTagContainer GetEquipTag() const { return EquipTagContainer; }
	FORCEINLINE FName GetComboAttackName() const { return ComboAttackName; }


	FORCEINLINE UAnimMontage* SetEquipMontageRight(UAnimMontage* Montage) { return EquipMontageRight = Montage; }
	FORCEINLINE UAnimMontage* SetEquipMontageLeft(UAnimMontage* Montage) { return EquipMontageLeft = Montage; }

	virtual void SetDefaultCombos() override;
	virtual void SetNextComboSegment(FName NextCombo) override;

};