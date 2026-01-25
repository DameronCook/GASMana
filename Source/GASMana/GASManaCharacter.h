// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameplayTagContainer.h"
#include <GameplayEffectTypes.h>
#include "AbilitySystemInterface.h"
#include "Public/ManaAttributeSet.h"
#include "Character/CharacterTypes.h"
#include "Interface/ComboInterface.h"
#include "Interface/HitStopInterface.h"
#include "Interface/IFocusableInterface.h"
#include "Interface/PlayerCharacterInterface.h"
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
class AGASManaCharacter : public ACharacter, public IAbilitySystemInterface, public IComboInterface, public IHitStopInterface, public IIFocusableInterface, public IPlayerCharacterInterface
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

	/////////////////////////////////////
	///Combat
	
	const FName DefaultComboName = "Attack01";
	FName ComboAttackName = DefaultComboName;
	bool bIsAttackWindowOpen;

	bool AmIAlive = true;

protected:

	/////////////////////////////////////////////////////////////////////
	///Equipment
	/**What Equipment the character has */
	UPROPERTY(VisibleAnywhere)
	EEquipmentState EquipmentState;
	
	UPROPERTY(VisibleAnywhere)
	ARightHandEquipment* RightHandEquipment;
	
	UPROPERTY(VisibleAnywhere)
	ALeftHandEquipment* LeftHandEquipment;

	/** Type of Equipment that the player is equipping*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> EquipmentTypeClass;
	
	/** Equip Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer EquipTagContainer;
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SetEquipment(AEquipment* Equipment);
	
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void UnSetEquipment(AGASManaCharacter* Char, AEquipment* Equipment);

	void RemoveAnyEquipClass() const;

	virtual bool GotMovementInput() const;
	
	virtual void SetOverlappingItem(class AItem* Item) override;

	void ChangeEquipmentDurability(AGASManaCharacter* Char, AEquipment* Equipment, int Amount);

	/////////////////////////////////////////////////////////////////////////
	///Combat
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void Blocking();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void FinishedBlocking();

	/** Runs on begin overlap */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual FGameplayTagContainer GetAttackType() const;

	/** Sets the attack montage for the attack to play */
	virtual void GetMontageToPlay();

	UFUNCTION()
	void PlayerCharacterDieMontage(const FVector& HitLocation);

	UPROPERTY()
	AItem* OverlappingItem = nullptr;

	UFUNCTION(Category = "Combat")
	virtual void GrabOverlappingItem();

	/** Pick Up Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PickUpMontage;

	
	/** Shield Stun Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer ShieldStunTagContainer;
	
	/** Attack Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer AttackTagContainer;

	UPROPERTY()
	UAnimMontage* CurrentAttackMontage;
	
	UPROPERTY()
	UGA_ManaPlayerAttack* ActiveAttackAbility;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ShieldStunMontage;

	UPROPERTY()
	FName NextAttackMontageSection = "Attack01";

	/** Attack Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer HitReactTagContainer;

	/** Current Hit React Montage*/
	UPROPERTY()
	UAnimMontage* HitReactMontage;

	/** Stationary React Montage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* StationaryReactMontage;

	/** Stationary React Montage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* MovingReactMontage;
	
	/** Attacking Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Attack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> AttackingEffectClass;

	/** Hit React Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Attack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> HitReactEffectClass;

	/** Death Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer DeathTagContainer;

	/** Death Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Death", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> DeathEffectClass;

	/** Death Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | ShieldStun", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> ShieldStunEffectClass;
	
	/** Death React Montage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Montage, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* DeathReactMontage;

	/** Death Direction */
	UPROPERTY()
	EDeathType DeathType;
	
	virtual void Die(const FVector& HitLocation);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cam, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraShakeBase> HitPlayerShieldCameraShake;

public:
	AGASManaCharacter();

	/////////////////////////////////////////////////////////////////////
	///Combat
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void MeleeAttackNotify(FVector AttackPosition, bool IsFinisher);

	virtual void SetDefaultCombos() override;
	virtual void SetNextComboSegment(FName NextCombo) override;

	UFUNCTION()
	bool IsAlive();

	UPROPERTY()
	bool bIsDead;

	UFUNCTION(BlueprintCallable)
	void Ragdoll();

	/**Called for attack by controller or by input */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual bool Attack();
	
	/** Called whenever the character is hit */
	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual void DirectionalHitReact(const FVector& HitterLocation, bool IsFinisher);

	FName HitReactSection;

	FName DeathReactSection;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector HitFlashColor = FVector::ZeroVector;

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

	UFUNCTION()
	void DropRightEquipment(ARightHandEquipment* Equipment);

	UFUNCTION()
	void DropLeftEquipment(ALeftHandEquipment* Equipment);

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

	virtual void RemoveFreeTag() const;

	virtual void LoadMe();

	virtual void UnloadMe();
	
	//////////////////////////////////////////////////////////////////////
	///Getters & Setters
	FORCEINLINE class UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	FORCEINLINE ARightHandEquipment* GetRightHandEquipment() const { return RightHandEquipment; }
	FORCEINLINE ALeftHandEquipment* GetLeftHandEquipment() const { return LeftHandEquipment; }
	
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDamageEffectClass() const { return DamageEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetEquipEffectClass() const { return EquipClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetAttackingEffectClass() const { return AttackingEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetHitReactEffectClass() const { return HitReactEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetShieldStunEffectClass() const { return ShieldStunEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDeathEffectClass() const { return DeathEffectClass; }

	
	FORCEINLINE UAnimMontage* GetHitReactMontage() const { return HitReactMontage; }
	FORCEINLINE FName GetHitReactSection() const { return HitReactSection; }

	FORCEINLINE UAnimMontage* GetShieldStunMontage() const { return ShieldStunMontage; }
	
	FORCEINLINE UAnimMontage* GetEquipRightMontage() const { return EquipMontageRight; }
	FORCEINLINE UAnimMontage* GetEquipLeftMontage() const { return EquipMontageLeft; }
	FORCEINLINE EEquipmentState GetEquipmentState() const { return EquipmentState; }
	FORCEINLINE FGameplayTagContainer GetEquipTag() const { return EquipTagContainer; }

	FORCEINLINE UGA_ManaPlayerAttack* GetAttackAbility() const { return ActiveAttackAbility; }
	FORCEINLINE FName GetComboAttackName() const { return ComboAttackName; }
	FORCEINLINE UAnimMontage* GetCurrentAttackMontage() const { return CurrentAttackMontage; }
	FORCEINLINE FName GetNextAttackMontageSection() const { return NextAttackMontageSection; }

	
	FORCEINLINE UAnimMontage* GetPickUpMontage() const { return PickUpMontage; }

	FORCEINLINE UAnimMontage* GetDeathMontage() const { return DeathReactMontage; }
	FORCEINLINE FName GetDeathMontageSection() const { return DeathReactSection; }
	FORCEINLINE EDeathType GetDeathType() const { return DeathType; }
	FORCEINLINE bool GetIsDead() const { return bIsDead; }
	FORCEINLINE EDeathType SetDeathType(const EDeathType NewType) { return DeathType = NewType; }
	
	FORCEINLINE UAnimMontage* SetEquipMontageRight(UAnimMontage* Montage) { return EquipMontageRight = Montage; }
	FORCEINLINE UAnimMontage* SetEquipMontageLeft(UAnimMontage* Montage) { return EquipMontageLeft = Montage; }
	FORCEINLINE EEquipmentState SetEquipmentState(const EEquipmentState State) { return EquipmentState = State; }
	FORCEINLINE UGA_ManaPlayerAttack* SetAttackAbility(UGA_ManaPlayerAttack* Attack) { return ActiveAttackAbility = Attack; }
	FORCEINLINE UAnimMontage* SetAttackMontage(UAnimMontage* UAttackMontage) { return CurrentAttackMontage = UAttackMontage; }
	FORCEINLINE UAnimMontage* SetHitReactMontage(UAnimMontage* AHitReactMontage) { return HitReactMontage = AHitReactMontage; }

};