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

//DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

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

	UPROPERTY()
	class UManaAttributeSet* Attributes;

public:
	AGASManaCharacter();
	//----------------------------------------------------------Ability System

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxHealth = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxMana = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes")
	int MaxStamina = 100;

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
};

