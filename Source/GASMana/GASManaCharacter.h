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

	/* Right Hand Equipment*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<AManaEquipmentParent> RightHandEquipment;

	/* Left Hand Equipment*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Equipment, meta = (AllowPrivateAccess = "true"))
	class TSubclassOf<AManaEquipmentParent> LeftHandEquipment;

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
	/* Adds Equipment to socket*/
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void AddEquipment(FName SocketName, TSubclassOf<AManaEquipmentParent> EquipmentClass);

	UFUNCTION(BlueprintCallable, Category = "Blocking")
	virtual void Blocking();

	UFUNCTION(BlueprintCallable, Category = "Blocking")
	virtual void FinishedBlocking();
public:


	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void InitializeAttributes();
	virtual void GiveDefaultAbilities();


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Abilities")
	TArray<TSubclassOf<class UGameplayAbility>> DefaultAbilities;

	FORCEINLINE class UAbilitySystemComponent* GetAbilitySystemComponent() const { return AbilitySystemComponent; }
	FORCEINLINE TSubclassOf<AManaEquipmentParent> GetRightHandEquipment() const { return RightHandEquipment; }
	FORCEINLINE TSubclassOf<AManaEquipmentParent> GetLeftHandEquipment() const { return LeftHandEquipment; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetDamageEffectClass() const { return DamageEffectClass; }
	FORCEINLINE UAnimMontage* GetHitReactMontage() const { return HitReactMontage; }
};

