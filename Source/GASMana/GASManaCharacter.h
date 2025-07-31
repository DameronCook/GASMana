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
#include "GASManaCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class AGASManaCharacter : public ACharacter, public IAbilitySystemInterface
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
	UAnimMontage* EquipMontage;

	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void PlayEquipMontage(const FName& SectionName);
	void SetEquipment();
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void AttatchWeaponToBack();
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void AttatchWeaponToHand();

	void RemoveAnyEquipClass();


public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void InitializeAttributes();
	virtual void GiveDefaultAbilities();
	
	UFUNCTION(BlueprintCallable)
	void PlayFlashEffect(FVector InColor, float FlashLength);

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

	FORCEINLINE class UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	FORCEINLINE TSubclassOf<AManaEquipmentParent> GetRightHandEquipment() const { return RightHandEquipmentClass; }
	FORCEINLINE TSubclassOf<AManaEquipmentParent> GetLeftHandEquipment() const { return LeftHandEquipmentClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDamageEffectClass() const { return DamageEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetEquipEffectClass() const { return EquipClass; }
	FORCEINLINE UAnimMontage* GetHitReactMontage() const { return HitReactMontage; }
	FORCEINLINE UAnimMontage* GetEquipMontage() const { return EquipMontage; }
	FORCEINLINE EEquipmentState GetEquipmentState() const { return EquipmentState; }
};