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
<<<<<<< HEAD
#include "Interface/HitStopInterface.h"
=======
#include "Interface/IFocusableInterface.h"
>>>>>>> Enemy-AI
#include "Item/Equipment.h"
#include "GASManaCharacter.generated.h"

class UGA_ManaPlayerAttack;
class ALeftHandEquipment;
class ARightHandEquipment;
class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
<<<<<<< HEAD
class AGASManaCharacter : public ACharacter, public IAbilitySystemInterface, public IComboInterface, public IHitStopInterface
=======
class AGASManaCharacter : public ACharacter, public IAbilitySystemInterface, public IComboInterface, public IIFocusableInterface
>>>>>>> Enemy-AI
{
	GENERATED_BODY()
	//////////////////////////////////////
	///Components 
	/** Ability Component */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	class UAbilitySystemComponent* AbilitySystemComponent;
	/** Attributes */
	UPROPERTY()
	class UManaAttributeSet* Attributes;

	//////////////////////////////////////
	///Equipment
	/** Damaged Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	/** Equip Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Equip", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> EquipClass;
		
	//////////////////////////////////////
	///Montages
	UPROPERTY()
	UAnimMontage* EquipMontageRight;
	UPROPERTY()
	UAnimMontage* EquipMontageLeft;
	/** Hit React Montage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	class UAnimMontage* HitReactMontage;

	/////////////////////////////////////
	///Combat
	
	const FName DefaultComboName = "Attack01";
	FName ComboAttackName = DefaultComboName;
	bool bIsAttackWindowOpen;

protected:

	/////////////////////////////////////////////////////////////////////
	///Equipment
	/**What Equipment the character has */
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EEquipmentState EquipmentState;
	
	UPROPERTY()
	ARightHandEquipment* RightHandEquipment;
	
	UPROPERTY()
	ALeftHandEquipment* LeftHandEquipment;

	/** Type of Equipment that the player is equipping*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> EquipmentTypeClass;
	
	/** Equip Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer EquipTagContainer;
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipment(AEquipment* Equipment);

	void RemoveAnyEquipClass() const;

	/////////////////////////////////////////////////////////////////////////
	///Combat
	UFUNCTION(BlueprintCallable, Category = "Blocking")
	virtual void Blocking();

	UFUNCTION(BlueprintCallable, Category = "Blocking")
	virtual void FinishedBlocking();

	/** Runs on begin overlap */
	UFUNCTION(BlueprintCallable, Category = "Overlap")
	virtual void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual FGameplayTagContainer GetAttackType() const;

	/** Sets the attack montage for the attack to play */
	virtual void GetMontageToPlay();

	/** Attack Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer AttackTagContainer;

	UPROPERTY()
	UAnimMontage* CurrentAttackMontage;
	
	UPROPERTY()
	UGA_ManaPlayerAttack* ActiveAttackAbility;

	UPROPERTY()
	FName NextAttackMontageSection = "Attack01";
	
	/** Attacking Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Attack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> AttackingEffectClass;


public:
	AGASManaCharacter();

	/////////////////////////////////////////////////////////////////////
	///Combat
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void HandleMelee();

	virtual void SetDefaultCombos() override;
	virtual void SetNextComboSegment(FName NextCombo) override;

	UFUNCTION()
	static bool IsAlive();

	/**Called for attack by controller or by input */
	UFUNCTION(BlueprintCallable, Category = "Attack")
	virtual bool Attack();

	////////////////////////////////////////////////////////////////////
	///Gear
	/** Instantly removes gear from hands */
	void InstantlyUnequipGear();
	
	/** Instantly Equips Gear to left hand*/
	UFUNCTION(BlueprintCallable)
	void EquipLeftHandGear();
	
	/** Instantly Equips Gear to Right hand*/
	UFUNCTION(BlueprintCallable)
	void EquipRightHandGear();
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void AttachWeaponToBack() const;
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void AttachWeaponToHand();

	void EquipGearToSocket(const AEquipment* GearToEquip, FName SocketName) const;

	//////////////////////////////////////////////////////////////////////
	///Attributes & Abilities

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

	virtual void InitializeAttributes();
	virtual void GiveDefaultAbilities();
	
	//////////////////////////////////////////////////////////////////////
	///Other
	UFUNCTION(BlueprintCallable)
	void PlayFlashEffect(FVector InColor, float FlashLength) const;
	

	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	//////////////////////////////////////////////////////////////////////
	///Getters & Setters
	FORCEINLINE class UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	FORCEINLINE ARightHandEquipment* GetRightHandEquipment() const { return RightHandEquipment; }
	FORCEINLINE ALeftHandEquipment* GetLeftHandEquipment() const { return LeftHandEquipment; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDamageEffectClass() const { return DamageEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetEquipEffectClass() const { return EquipClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetAttackingEffectClass() const { return AttackingEffectClass; }
	FORCEINLINE UAnimMontage* GetHitReactMontage() const { return HitReactMontage; }
	FORCEINLINE UGA_ManaPlayerAttack* GetAttackAbility() const { return ActiveAttackAbility; }
	FORCEINLINE UAnimMontage* GetEquipRightMontage() const { return EquipMontageRight; }
	FORCEINLINE UAnimMontage* GetEquipLeftMontage() const { return EquipMontageLeft; }
	FORCEINLINE EEquipmentState GetEquipmentState() const { return EquipmentState; }
	FORCEINLINE FGameplayTagContainer GetEquipTag() const { return EquipTagContainer; }
	FORCEINLINE FName GetComboAttackName() const { return ComboAttackName; }
	FORCEINLINE UAnimMontage* GetCurrentAttackMontage() const { return CurrentAttackMontage; }
	FORCEINLINE FName GetNextAttackMontageSection() const { return NextAttackMontageSection; }

	
	FORCEINLINE UAnimMontage* SetEquipMontageRight(UAnimMontage* Montage) { return EquipMontageRight = Montage; }
	FORCEINLINE UAnimMontage* SetEquipMontageLeft(UAnimMontage* Montage) { return EquipMontageLeft = Montage; }
	FORCEINLINE EEquipmentState SetEquipmentState(const EEquipmentState State) { return EquipmentState = State; }
	FORCEINLINE UGA_ManaPlayerAttack* SetAttackAbility(UGA_ManaPlayerAttack* Attack) { return ActiveAttackAbility = Attack; }
	FORCEINLINE UAnimMontage* SetAttackMontage(UAnimMontage* UAttackMontage) { return CurrentAttackMontage = UAttackMontage; }

};