// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GASManaCharacter.h"
#include "../../GameplayTags/Classes/GameplayTagContainer.h"
#include "PlayerManaCharacter.generated.h"


class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
struct FGameplayTagContainer;

/**
 * 
 */
UCLASS()
class GASMANA_API APlayerManaCharacter : public AGASManaCharacter
{
	GENERATED_BODY()
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AttackAction;

	/** Block Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BlockAction;

	/** Attack Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RollAction;

	/** Jump Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer JumpTagContainer;

	/** Block Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer BlockTagContainer;

	/** Roll Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer RollTagContainer;

	/** Grounded Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> GroundedEffectClass;

	/** Airborne Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> AirborneEffectClass;

	/** Blocking Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> BlockingEffectClass;

	/** Rolling Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> RollingEffectClass;

	/** Rolling Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roll", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollMontage;

	/** Roll MovementCurve */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roll", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* DiveRollCurveFloat;

	UFUNCTION()
	void OnBlockingTagChanged(const FGameplayTag Tag, int32 NewCount);

protected:

	virtual void BeginPlay() override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviouisCustomMode) override;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void FinishedBlocking() override;

	virtual void Jump() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/**Called for attack input */
	void Attack(const FInputActionValue& Value);

	/**Called for attack input */
	void Block(const FInputActionValue& Value);
	void StopBlock(const FInputActionValue& Value);

	/**Called for attack input */
	void Roll(const FInputActionValue& Value);

public:
	APlayerManaCharacter();

	virtual void Blocking() override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/** Returns BlockingEffectClass **/
	FORCEINLINE TSubclassOf<UGameplayEffect> GetBlockingEffectClass() const { return BlockingEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetRollingEffectClass() const { return RollingEffectClass; }
	FORCEINLINE UAnimMontage* GetRollMontage() const { return RollMontage; }
	FORCEINLINE UCurveFloat* GetDiveRollCurveFloat() const { return DiveRollCurveFloat; }
};
