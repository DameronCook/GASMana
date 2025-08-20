// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include <GameplayEffectTypes.h>
#include "AbilitySystemInterface.h"
#include "Public/ManaAttributeSet.h"
#include "Public/Actors/Equipment/ManaEquipmentParent.h"
#include "Logging/LogMacros.h"
#include "Character/CharacterTypes.h"
#include "Interface/ComboInterface.h"
<<<<<<< Updated upstream
=======
#include "Item/Equipment.h"
#include "Item/RightHandEquipment.h"
>>>>>>> Stashed changes
#include "GASManaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class AGASManaCharacter : public ACharacter, public IAbilitySystemInterface, public IComboInterface
{
	GENERATED_BODY()

	/** Ability Component*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	class UAbilitySystemComponent* AbilitySystemComponent;

	/** Hit React Montage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HitReactMontage;

	////////////////////////////////////////////////////////////////
	/// Attributes

	UPROPERTY()
	class UManaAttributeSet* Attributes;

<<<<<<< Updated upstream

	/** Damaged Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> DamageEffectClass;

public:
	AGASManaCharacter();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleMelee();

protected:

	/* Right Hand Equipment*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<AManaEquipmentParent> RightHandEquipmentClass;

	/* Left Hand Equipment*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<AManaEquipmentParent> LeftHandEquipmentClass;

	AManaEquipmentParent* RightHandEquipment;
	AManaEquipmentParent* LeftHandEquipment;

	/* Adds Equipment to socket*/
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	AManaEquipmentParent* AddEquipment(FName SocketName, TSubclassOf<AManaEquipmentParent> EquipmentClass);

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
	//Equipment
	/** Equip Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontageRight;

	/** Equip Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* EquipMontageLeft;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void PlayEquipMontage(const FName& SectionName);
	void SetEquipment();

	void RemoveAnyEquipClass();

	const FName DefaultComboName = "Attack01";
	FName ComboAttackName = DefaultComboName;
	bool bIsAttackWindowOpen;


public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	void InstantlyUnequipGear();
	/* Instantly Equips Gear to left hand*/
	void EquipLeftHandGear();
	virtual void InitializeAttributes();
	virtual void GiveDefaultAbilities();
	
	UFUNCTION(BlueprintCallable)
	void PlayFlashEffect(FVector InColor, float FlashLength);
=======
	////////////////////////////////////////////////////////////////
	///Equipment
	UPROPERTY()
	ARightHandEquipment* RightHandEquipment;
	UPROPERTY()
	ALeftHandEquipment* LeftHandEquipment;
	
	/* Instantly Equips Gear to left hand*/
	UFUNCTION(BlueprintCallable)
	void EquipLeftHandGear();
	
	UFUNCTION(BlueprintCallable)
	void EquipRightHandGear();
>>>>>>> Stashed changes

	UPROPERTY()
	UAnimMontage* EquipMontageRight;
	
	UPROPERTY()
	UAnimMontage* EquipMontageLeft;

	UFUNCTION(Category = "Equipment")
	void RemoveAnyEquipClass() const;

	UFUNCTION(Category = "Equipment")
	void EquipGearToSocket(const AEquipment* GearToEquip, FName SocketName) const;

	/** Type of Equipment that the player is equipping*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> EquipmentTypeClass;
	
	/** Damaged Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	/** Equip Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Equip", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> EquipClass;

	/** Equip Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer EquipTagContainer;

	/** What Equipment the character has */
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EEquipmentState EquipmentState;

	////////////////////////////////////////////////////////////////
	///Melee
	const FName DefaultComboName = "Attack01";
	FName ComboAttackName = DefaultComboName;
	bool bIsAttackWindowOpen;

protected:
	
	/** Runs on begin overlap */
	UFUNCTION(BlueprintCallable, Category = "Overlap")
	virtual void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	////////////////////////////////////////////////////////////////
	///Equipment
	UFUNCTION(Category = "Equipment")
	void SetEquipment(const AEquipment* Equipment);
	
	////////////////////////////////////////////////////////////////
	///Melee
	UFUNCTION(BlueprintCallable, Category = "Blocking")
	virtual void Blocking();

	UFUNCTION(BlueprintCallable, Category = "Blocking")
	virtual void FinishedBlocking();
	

public:
	AGASManaCharacter();

	/** Immediately unequips gear from the player */
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void InstantlyUnequipGear();

	////////////////////////////////////////////////////////////////
	///Melee
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleMelee();
	virtual void SetDefaultCombos() override;
	virtual void SetNextComboSegment(FName NextCombo) override;

	////////////////////////////////////////////////////////////////
	///Equipment
	UFUNCTION(BlueprintCallable, Category = "Equipment")
<<<<<<< Updated upstream
	void AttatchWeaponToBack();
=======
	void AttachWeaponToBack() const;
	
>>>>>>> Stashed changes
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void AttatchWeaponToHand();

<<<<<<< Updated upstream
	void EquipGearToSocket(AManaEquipmentParent* GearToEquip, FName SocketName);
=======
	////////////////////////////////////////////////////////////////
	///Attributes & Abilities
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void InitializeAttributes();
	virtual void GiveDefaultAbilities();
>>>>>>> Stashed changes

	/** Default Attributes given to the character */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;
	
	/** Current Abilities the player has */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

	////////////////////////////////////////////////////////////////
	///Other
	UFUNCTION()
	void PlayFlashEffect(FVector InColor, float FlashLength) const;

	////////////////////////////////////////////////////////////////
	///Getters/Setters
	/* Getters */
	FORCEINLINE class UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	FORCEINLINE TSubclassOf<AManaEquipmentParent> GetRightHandEquipment() const { return RightHandEquipmentClass; }
	FORCEINLINE TSubclassOf<AManaEquipmentParent> GetLeftHandEquipment() const { return LeftHandEquipmentClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDamageEffectClass() const { return DamageEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetEquipEffectClass() const { return EquipClass; }
	FORCEINLINE UAnimMontage* GetHitReactMontage() const { return HitReactMontage; }
	FORCEINLINE UAnimMontage* GetEquipRightMontage() const { return EquipMontageRight; }
	FORCEINLINE UAnimMontage* GetEquipLeftMontage() const { return EquipMontageLeft; }
	FORCEINLINE EEquipmentState GetEquipmentState() const { return EquipmentState; }
	FORCEINLINE FGameplayTagContainer GetEquipTag() const { return EquipTagContainer; }
	FORCEINLINE FName GetComboAttackName() const { return ComboAttackName; }

<<<<<<< Updated upstream


	virtual void SetDefaultCombos() override;
	virtual void SetNextComboSegment(FName NextCombo) override;
=======
	/* Setters */
	FORCEINLINE UAnimMontage* SetEquipMontageRight(UAnimMontage* Montage) { return EquipMontageRight = Montage; }
	FORCEINLINE UAnimMontage* SetEquipMontageLeft(UAnimMontage* Montage) { return EquipMontageLeft = Montage; }
	FORCEINLINE ARightHandEquipment* SetRightHandEquipment(ARightHandEquipment* RightHandGear) { return RightHandEquipment = RightHandGear; }
	FORCEINLINE ALeftHandEquipment* SetLeftHandEquipment(ALeftHandEquipment* LeftHandGear) { return LeftHandEquipment = LeftHandGear; }
	FORCEINLINE EEquipmentState SetEquipmentState(const EEquipmentState EquipState) { return EquipmentState = EquipState; }
>>>>>>> Stashed changes

};