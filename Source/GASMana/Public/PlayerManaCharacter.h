// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GASManaCharacter.h"
#include "../../GameplayTags/Classes/GameplayTagContainer.h"
#include "Interface/I_ProgressBarInterface.h"
#include "Interface/CameraActorInterface.h"
#include "Interface/TargetingActorInterface.h"
#include "Interface/PlayerCharacterInterface.h"
#include "Ability/GA_ManaPlayerWallRun.h"
#include "Ability/GA_ManaPlayerMantle.h"
#include "Ability/GA_ManaPlayerZipToPoint.h"
#include "Ability/GA_ManaPlayerSwing.h"
#include "Ability/GA_ManaPlayerHook.h"
#include "Components/AC_HookShot.h"
#include "Components/AC_WallRun.h"
#include "PlayerManaCharacter.generated.h"


class UGA_ManaPlayerFocus;
class UManaSpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UGA_ManaPlayerAirAttack;
class UGA_ManaPlayerAttack;
struct FInputActionValue;
struct FGameplayTagContainer;

UCLASS()
class GASMANA_API APlayerManaCharacter : public AGASManaCharacter, public II_ProgressBarInterface, public ICameraActorInterface, public IPlayerCharacterInterface , public ITargetingActorInterface
{
	GENERATED_BODY()

	//////////////////////////////////////////////////////////////////////////
	// Components
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UManaSpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Hook Shot Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	UAC_HookShot* HookShotComponent;

	/** Wall Run Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	UAC_WallRun* WallRunComponent;

	/** Player HUD */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
	UUserWidget* PlayerHUD;

	/** Player HUD Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = HUD, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> PlayerHUDClass;


	//////////////////////////////////////////////////////////////////////////
	// Input

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

	/** Roll Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* RollAction;

	/** Hook Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* HookAction;

	/** Hook Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EquipAction;

	/* Cached direction of input (always calculated) */
	FVector CachedInputDirection;

	//////////////////////////////////////////////////////////////////////////
	// Abilities

	/** Jump Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer JumpTagContainer;

	/** Block Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer BlockTagContainer;

	/** Roll Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer RollTagContainer;

	/** Air Attack Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer AirAttackTagContainer;

	/** Wall Run Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer WallRunTagContainer;

	/** Wall Jump Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer WallJumpTagContainer;

	/** Wall Jump Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer MantleTagContainer;

	/** Zip To Point Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer ZipToPointTagContainer;

	/** Swing Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer SwingTagContainer;

	/** Swing Jump Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer SwingJumpTagContainer;

	/** Hook Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer HookTagContainer;
	
	/** Focus Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer FocusTagContainer;

	//////////////////////////////////////////////////////////////////////////
	// Effect Classes

	/** Grounded Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Movement", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> GroundedEffectClass;

	/** Airborne Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Movement", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> AirborneEffectClass;

	/** Blocking Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Block", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> BlockingEffectClass;

	/** Rolling Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Roll", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> RollingEffectClass;

	/** Air Attacking Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Attack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> AirAttackEffectClass;

	/** Free to Move Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Movement", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> FreeEffectClass;

	/** Block Movement Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Movement", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> BlockMovementEffectClass;

	/** Wall Run Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Movement", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> WallRunEffectClass;

	/** Wall Jump Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Movement", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> WallJumpEffectClass;

	/** Normal Stamina Regen Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Stamina", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> StaminaRegenEffectClass;

	/** Block Stamina Regen Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Stamina", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> StaminaRegenBlockEffectClass;

	/** Mana Drain Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Mana", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> ManaWallRunDrainClass;

	/** Mantle Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Movement", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> MantleClass;

	/** ZipToPoint Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Hook", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> ZipToPointClass;

	/** Swing Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Hook", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> SwingClass;

	/** Swing Jump Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Hook", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> SwingJumpClass;

	/** Hook Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Hook", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> HookClass;
	
	/** Focus Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> FocusClass;

	//////////////////////////////////////////////////////////////////////////
	// Montages

	/** Rolling Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollMontage;

	/** Zip To Point Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ZipToPointMontage;

	/** Swing Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* SwingMontage;
	
	/** Equip Attack Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AirAttackMontage;

	/** Pick Up Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PickUpMontage;

	/** Shield Block Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ShieldBlockMontage;

	/** Throw Hook Montage to play */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ThrowHookMontage;
	
	UPROPERTY()
	UAnimMontage* CurrentBlockingMontage;


	//////////////////////////////////////////////////////////////////////////
	// Curve Floats

	/** Roll MovementCurve */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* DiveRollCurveFloat;

	/** Zip to Point MovementCurve */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ZipToPointCurveFloat;

	/** Jump MovementCurve */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* JumpCurveFloat;

	//////////////////////////////////////////////////////////////////////////
	// Object References (Liable to move)

	/** Enemy Base */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGASManaCharacter> BaseEnemy;

	UFUNCTION()
	void OnBlockingTagChanged(const FGameplayTag Tag, int32 NewCount);

	////////////////////////////////////////////
	//Swing Variables
	UFUNCTION()
	FVector GamepadRightSwingForce(float MovementInput) const;
	UFUNCTION()
	FVector GamepadForwardSwingForce(float MovementInput) const;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swing", meta = (AllowPrivateAccess = "true"))
	float SwingSpeedBalancer = 20.f;


	//////////////////////////////////////////
	//Active Abilities

	/**Active Wall run ability*/
	UPROPERTY()
	UGA_ManaPlayerWallRun* ActiveWallRunAbility = nullptr;

	/**Active mantle ability*/
	UPROPERTY()
	UGA_ManaPlayerMantle* ActiveMantleAbility = nullptr;

	/**Active mantle ability*/
	UPROPERTY()
	UGA_ManaPlayerHook* ActiveHookAbility = nullptr;

	/**Active mantle ability*/
	UPROPERTY()
	UGA_ManaPlayerZipToPoint* ActiveZipAbility = nullptr;

	/**Active mantle ability*/
	UPROPERTY()
	UGA_ManaPlayerSwing* ActiveSwingAbility = nullptr;

	/**Active Air Attack ability*/
	UPROPERTY()
	UGA_ManaPlayerAirAttack* ActiveAirAttackAbility = nullptr;
	
	/**Active Focus ability*/
	UPROPERTY()
	UGA_ManaPlayerFocus* ActiveFocusAbility = nullptr;

	/////////////////////////////////////////////////////////////////////////
	//Other
	float OriginalGravityScale = 2.0;

	//Cam stuff....
	UPROPERTY()
	AManaCameraModificationVolume* CurrentCameraModificationVolume;

	bool bShouldAddCameraTarget = false;

	UPROPERTY()
	AActor* CurrentCamTarget = nullptr;

	UPROPERTY()
	AActor* CombatCameraTarget = nullptr;

	UPROPERTY(VisibleDefaultsOnly)
	AItem* OverlappingItem = nullptr;

	UPROPERTY()
	AActor* OverlappingItemActor = nullptr;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void FinishedBlocking() override;

	virtual void Jump() override;

	UFUNCTION(BlueprintCallable)
	void OnMantleEnded();

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/**Called for attack input */
	virtual bool Attack() override;

	void AttackInput(const FInputActionValue& Value);

	virtual FGameplayTagContainer GetAttackType() const override;
	virtual void GetMontageToPlay() override;
	
	/**Called for block input */
	void Block(const FInputActionValue& Value);
	void StopBlock(const FInputActionValue& Value);

	/**Called for roll input */
	void Roll(const FInputActionValue& Value);

	/**Called for hook input */
	void Hook(const FInputActionValue& Value);
	void GrabOverlappingItem();

	/**Called for Equip input */
	void Equip(const FInputActionValue& Value);

public:
	APlayerManaCharacter();

	virtual void Blocking() override;

	virtual void HandleMelee() override;

	void UpdateStaminaRegen() const;

	//Interface overrides
	///////////////////////////////////////
	virtual float GetHealth_Implementation() const override;
	virtual float GetHealthAsRatio_Implementation() const override;
	virtual float GetStamina_Implementation() const override;
	virtual float GetStaminaAsRatio_Implementation() const override;
	virtual float GetMana_Implementation() const override;
	virtual float GetManaAsRatio_Implementation() const override;


	//Attack Interface
	virtual void SetDefaultCombos() override;
	virtual void SetNextComboSegment(FName NextCombo) override;

	virtual void SetOverlappingItem(class AItem* Item) override;

	//////////////////////////////////////
	//Camera Functions

	virtual AManaCameraModificationVolume* GetCurrentCameraModificationVolume() const override;
	virtual void SetCurrentCameraModificationVolume(AManaCameraModificationVolume* InCurrentCameraModificationVolume) override;
	virtual bool GotMovementInput() const override;

	virtual bool SelectTarget(bool SelectTarget) override;
	virtual AActor* SetCurrentTarget(AActor* CurrentTarget) override;
	virtual AActor* GetCurrentTarget() const override;
	virtual bool IsSelectingTarget() const override;


	//////////////////////////////////////
	//Convenience functions
	/* Remove the free tag from the player */
	void RemoveFreeTag() const;

	//////////////////////////////////////
	//Getters
	// 
	//Components
	FORCEINLINE class UManaSpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE class UAC_HookShot* GetHookShot() const { return HookShotComponent; }
	FORCEINLINE class UAC_WallRun* GetWallRun() const { return WallRunComponent; }

	//Effect Classes
	FORCEINLINE TSubclassOf<UGameplayEffect> GetBlockingEffectClass() const { return BlockingEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetRollingEffectClass() const { return RollingEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetAirAttackEffectClass() const { return AirAttackEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetFreeEffectClass() const { return FreeEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetBlockMovementEffectClass() const { return BlockMovementEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetWallRunEffectClass() const { return WallRunEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetWallJumpEffectClass() const { return WallJumpEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetStaminaRegenEffectClass() const { return StaminaRegenEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetStaminaRegenBlockEffectClass() const { return StaminaRegenBlockEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetWallRunDrainEffectClass() const { return ManaWallRunDrainClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetAirborneEffectClass() const { return AirborneEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetZipToPointEffectClass() const { return ZipToPointClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetSwingEffectClass() const { return SwingClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetSwingJumpEffectClass() const { return SwingJumpClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetHookEffectClass() const { return HookClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetMantleEffectClass() const { return MantleClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetFocusEffectClass() const { return FocusClass; }

	//Ability Classes and Tags
	FORCEINLINE FGameplayTagContainer GetZipToPointTag() const { return ZipToPointTagContainer; }
	FORCEINLINE FGameplayTagContainer GetSwingTag() const { return SwingTagContainer; }
	FORCEINLINE FGameplayTagContainer GetWallRunTag() const { return WallRunTagContainer; }
	FORCEINLINE FGameplayTagContainer GetBlockTag() const { return BlockTagContainer; }

	//Montages
	FORCEINLINE UAnimMontage* GetRollMontage() const { return RollMontage; }
	FORCEINLINE UAnimMontage* GetAirAttackMontage() const { return AirAttackMontage; }
	FORCEINLINE UAnimMontage* GetZipToPointMontage() const { return ZipToPointMontage; }
	FORCEINLINE UAnimMontage* GetSwingMontage() const { return SwingMontage; }
	FORCEINLINE UAnimMontage* GetPickUpMontage() const { return PickUpMontage; }
	FORCEINLINE UAnimMontage* GetThrowHookMontage() const { return ThrowHookMontage; }

	FORCEINLINE UCurveFloat* GetDiveRollCurveFloat() const { return DiveRollCurveFloat; }
	FORCEINLINE UCurveFloat* GetZipToPointCurveFloat() const { return ZipToPointCurveFloat; }
	FORCEINLINE UCurveFloat* GetJumpCurve() const { return JumpCurveFloat; }


	//Input Actions
	FORCEINLINE UInputAction* GetMoveAction() const { return MoveAction; }
	FORCEINLINE FVector GetCachedInputDirection() const { return CachedInputDirection; }

	//Movement
	FORCEINLINE UGA_ManaPlayerWallRun* GetWallRunAbility() const { return ActiveWallRunAbility; }
	FORCEINLINE UGA_ManaPlayerMantle* GetMantleAbility() const { return ActiveMantleAbility; }
	FORCEINLINE UGA_ManaPlayerHook* GetHookAbility() const { return ActiveHookAbility; }
	FORCEINLINE UGA_ManaPlayerZipToPoint* GetZipAbility() const { return ActiveZipAbility; }
	FORCEINLINE UGA_ManaPlayerSwing* GetSwingAbility() const { return ActiveSwingAbility; }
	FORCEINLINE UGA_ManaPlayerAirAttack* GetAirAttackAbility() const { return ActiveAirAttackAbility; }
	FORCEINLINE UGA_ManaPlayerFocus* GetFocusAbility() const { return ActiveFocusAbility; }

	//Swinging stuff
	//Move to another component?
	FORCEINLINE float GetSwingSpeedBalancer() const { return SwingSpeedBalancer; }
	FORCEINLINE float GetOriginalGravityScale() const { return OriginalGravityScale; }


	FORCEINLINE AActor* GetCombatCameraTarget() const { return CombatCameraTarget; }


	//////////////////////////////////////
	//Setters
	FORCEINLINE UGA_ManaPlayerWallRun* SetWallRunAbility(UGA_ManaPlayerWallRun* WallRunAbility) { return ActiveWallRunAbility = WallRunAbility; }
	FORCEINLINE UGA_ManaPlayerMantle* SetMantleAbility(UGA_ManaPlayerMantle* MantleAbility) { return ActiveMantleAbility = MantleAbility; }
	FORCEINLINE UGA_ManaPlayerHook* SetHookAbility(UGA_ManaPlayerHook* HookAbility) { return ActiveHookAbility = HookAbility; }
	FORCEINLINE UGA_ManaPlayerZipToPoint* SetZipToPointAbility(UGA_ManaPlayerZipToPoint* ZipAbility) { return ActiveZipAbility = ZipAbility; }
	FORCEINLINE UGA_ManaPlayerSwing* SetSwingAbility(UGA_ManaPlayerSwing* SwingAbility) { return ActiveSwingAbility = SwingAbility; }
	FORCEINLINE UGA_ManaPlayerAirAttack* SetAirAttackAbility(UGA_ManaPlayerAirAttack* AirAttack) { return ActiveAirAttackAbility = AirAttack; }
	FORCEINLINE UGA_ManaPlayerFocus* SetFocusAbility(UGA_ManaPlayerFocus* FocusAbility) { return ActiveFocusAbility = FocusAbility; }
	FORCEINLINE AActor* SetCombatCameraTarget(AActor* Target) { return CombatCameraTarget = Target; }
};
