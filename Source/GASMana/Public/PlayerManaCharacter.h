// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../GASManaCharacter.h"
#include "../../GameplayTags/Classes/GameplayTagContainer.h"
#include "Interface/I_ProgressBarInterface.h"
#include "Ability/GA_ManaPlayerWallRun.h"
#include "Ability/GA_ManaPlayerMantle.h"
#include "Ability/GA_ManaPlayerHook.h"
#include "Ability/GA_ManaPlayerZipToPoint.h"
#include "Ability/GA_ManaPlayerSwing.h"
#include "Components/AdvancedCameraComponent.h"
#include "Components/AC_HookShot.h"
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
UENUM(BlueprintType)
enum class EWallRunSide : uint8
{
	Left  UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};

UCLASS()
class GASMANA_API APlayerManaCharacter : public AGASManaCharacter, public II_ProgressBarInterface
{
	GENERATED_BODY()

	//////////////////////////////////////////////////////////////////////////
	// Components
	
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	/** Camera Controller Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UAdvancedCameraComponent* AdvancedCameraComponent;

	/** Hook Shot Component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	UAC_HookShot* HookShotComponent;

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

	/** Attack Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer AttackTagContainer;

	/** Wall Run Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer WallRunTagContainer;

	/** Wall Jump Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer WallJumpTagContainer;

	/** Mantle Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer MantleTagContainer;

	/** Hook Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer HookTagContainer;

	/** Zip To Point Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer ZipToPointTagContainer;

	/** Swing Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer SwingTagContainer;

	/** Swing Tag Container */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities, meta = (AllowPrivateAccess = "true"))
	FGameplayTagContainer SwingJumpTagContainer;

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

	/** Rolling Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Attack", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> AttackingEffectClass;

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

	/** Hook Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Hook", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> HookClass;

	/** Zip To Point Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Hook", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> ZipToPointClass;

	/** Swing Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Hook", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> SwingClass;

	/** Swing Effect Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities | Hook", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> SwingJumpClass;

	//////////////////////////////////////////////////////////////////////////
	// Montages

	/** Rolling Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* RollMontage;

	/** Atack Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* AttackMontage;

	/** Wall Run Right Montage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* WallRunLeftMontage;

	/** Wall Run Right Montage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* WallJumpMontage;

	/** Wall Run left Montage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* WallRunRightMontage;

	/** Wall Run Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PlayableWallRunMontage;



	//////////////////////////////////////////////////////////////////////////
	// Curve Floats

	/** Roll MovementCurve */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* DiveRollCurveFloat;

	/** Wall Jump MovementCurve */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* WallJumpCurveFloat;

	/** Wall Jump MovementCurve */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* ZipToPointCurveFloat;

	//////////////////////////////////////////////////////////////////////////
	// Object References (Liable to move)

	/** Enemy Base */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Combat", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGASManaCharacter> BaseEnemy;

	UFUNCTION()
	void OnBlockingTagChanged(const FGameplayTag Tag, int32 NewCount);

	//////////////////////////////////////////////////////////////////////////
	// Wall Run

	/** Wall Run Side */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Run", meta = (AllowPrivateAccess = "true"))
	EWallRunSide WallRunSide;

	/** Wall Run Direction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Run", meta = (AllowPrivateAccess = "true"))
	FVector WallRunDirection;

	/** Wall Run Direction */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Run", meta = (AllowPrivateAccess = "true"))
	FVector WallRunDir;

	/** Wall Run Speed */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Run", meta = (AllowPrivateAccess = "true"))
	float WallRunStrength = 800.f;

	/* Duration of Full Wall Run */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Run", meta = (AllowPrivateAccess = "true"))
	float WallRunDuration = 2.0f;

	/* Verticality of the  Wall Run */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Run", meta = (AllowPrivateAccess = "true"))
	float WallRunAmplitude = 125.0f;

	/* How much along the sine curve do we follow? */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Wall Run", meta = (AllowPrivateAccess = "true"))
	float WallRunFreq = 0.55f;


	float WallRunElapsedTime = 0.0f;
	float WallRunStartZ = 0.0f;
	FVector WallRunImpactNormal;

	float LastWallRunSineOffset = 0.0f;

	UFUNCTION()
	FVector UpdateWallRunHorizontal();

	UFUNCTION()
	void UpdateWallRunVertical(float DeltaTime);

	UFUNCTION()
	bool WallRunCheck();

	UFUNCTION()
	bool CanWallRunOnSurface(FVector ImpactNormal);

	UFUNCTION()
	bool IsWallRunningAlongRightSide(FVector ImpactNormal);

	UFUNCTION()
	bool ForwardWallRunCheck();

	UFUNCTION()
	FVector SetWallRunDirection(FVector SideMultiplier, FVector ImpactNormal);

	////////////////////////////////////////////
	//Swing Variables
	UFUNCTION()
	FVector GamepadRightSwingForce(float MovementInput);
	UFUNCTION()
	FVector GamepadForwardSwingForce(float MovementInput);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Swing", meta = (AllowPrivateAccess = "true"))
	float SwingSpeedBalancer = 12.f;


	//////////////////////////////////////////
	//Active Abilities

	/**Active wall run ability*/
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UGA_ManaPlayerWallRun* ActiveWallRunAbility = nullptr;

	/**Active mantle ability*/
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UGA_ManaPlayerMantle* ActiveMantleAbility = nullptr;

	/**Active hook ability*/
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UGA_ManaPlayerHook* ActiveHookAbility = nullptr;

	/**Active zip ability*/
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UGA_ManaPlayerZipToPoint* ActiveZipAbility = nullptr;

	/**Active swing ability*/
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UGA_ManaPlayerSwing* ActiveSwingAbility = nullptr;

	/////////////////////////////////////////////////////////////////////////
	//Other
	float OriginalGravityScale = 2.0;

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviouisCustomMode) override;

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
	void Attack(const FInputActionValue& Value);

	/**Called for attack input */
	void Block(const FInputActionValue& Value);
	void StopBlock(const FInputActionValue& Value);

	/**Called for roll input */
	void Roll(const FInputActionValue& Value);

	/**Called for hook input */
	void Hook(const FInputActionValue& Value);

public:
	APlayerManaCharacter();

	virtual void Blocking() override;

	virtual void HandleMelee() override;

	void UpdateStaminaRegen();

	//Interface overrides
	///////////////////////////////////////
	virtual float GetHealth_Implementation() const override;
	virtual float GetHealthAsRatio_Implementation() const override;
	virtual float GetStamina_Implementation() const override;
	virtual float GetStaminaAsRatio_Implementation() const override;
	virtual float GetMana_Implementation() const override;
	virtual float GetManaAsRatio_Implementation() const override;

	//////////////////////////////////////
	//Camera States

	UFUNCTION(Category = "Camera")
	void SetDefaultCameraState();

	UFUNCTION(Category = "Camera")
	void SetWallRunCameraState();

	UFUNCTION(Category = "Camera")
	void SetZipToPointCameraState();

	UFUNCTION(Category = "Camera")
	void SetSwingCameraState();

	UFUNCTION(Category = "Camera")
	void SetRollCameraState();

	UFUNCTION(Category = "Camera")
	void SetWallJumpCameraState();

	UFUNCTION(Category = "Camera")
	void SetShieldCameraState();

	//////////////////////////////////////
	//Getters
	// 
	//Components
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE class UAC_HookShot* GetHookShot() const { return HookShotComponent; }

	//Effect Classes
	FORCEINLINE TSubclassOf<UGameplayEffect> GetBlockingEffectClass() const { return BlockingEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetRollingEffectClass() const { return RollingEffectClass; }
	FORCEINLINE TSubclassOf<UGameplayEffect> GetAttackingEffectClass() const { return AttackingEffectClass; }
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

	//Ability Classes and Tags
	FORCEINLINE FGameplayTagContainer GetZipToPointTag() const { return ZipToPointTagContainer; }
	FORCEINLINE FGameplayTagContainer GetSwingTag() const { return SwingTagContainer; }

	//Montages
	FORCEINLINE UAnimMontage* GetRollMontage() const { return RollMontage; }
	FORCEINLINE UAnimMontage* GetAttackMontage() const { return AttackMontage; }
	FORCEINLINE UAnimMontage* GetWallRunMontage() const { return PlayableWallRunMontage; }
	FORCEINLINE UAnimMontage* GetWallJumpMontage() const { return WallJumpMontage; }
	FORCEINLINE UCurveFloat* GetDiveRollCurveFloat() const { return DiveRollCurveFloat; }
	FORCEINLINE UCurveFloat* GetWallJumpCurveFloat() const { return WallJumpCurveFloat; }
	FORCEINLINE UCurveFloat* GetZipToPointCurveFloat() const { return ZipToPointCurveFloat; }

	//Input Actions
	FORCEINLINE UInputAction* GetMoveAction() const { return MoveAction; }
	FORCEINLINE FVector GetCachedInputDirection() const { return CachedInputDirection; }

	//Movement
	FORCEINLINE FVector GetWallRunDirection() const { return WallRunDirection; }
	FORCEINLINE FVector GetWallRunImpactNormal() const { return WallRunImpactNormal; }
	FORCEINLINE UGA_ManaPlayerWallRun* GetWallRunAbility() { return ActiveWallRunAbility; }
	FORCEINLINE UGA_ManaPlayerMantle* GetMantleAbility() { return ActiveMantleAbility; }
	FORCEINLINE UGA_ManaPlayerHook* GetHookAbility() { return ActiveHookAbility; }
	FORCEINLINE UGA_ManaPlayerZipToPoint* GetZipAbility() { return ActiveZipAbility; }
	FORCEINLINE UGA_ManaPlayerSwing* GetSwingAbility() { return ActiveSwingAbility; }
	FORCEINLINE EWallRunSide GetWallRunSide() { return WallRunSide; }
	FORCEINLINE float GetOriginalGravityScale() const { return OriginalGravityScale; }
	FORCEINLINE float GetSwingSpeedBalancer() const { return SwingSpeedBalancer; }


	//////////////////////////////////////
	//Setters
	FORCEINLINE UGA_ManaPlayerWallRun* SetWallRunAbility(UGA_ManaPlayerWallRun* WallRunAbility) { return ActiveWallRunAbility = WallRunAbility; }
	FORCEINLINE UGA_ManaPlayerMantle* SetMantleAbility(UGA_ManaPlayerMantle* MantleAbility) { return ActiveMantleAbility = MantleAbility; }
	FORCEINLINE UGA_ManaPlayerHook* SetHookAbility(UGA_ManaPlayerHook* HookAbility) { return ActiveHookAbility = HookAbility; }
	FORCEINLINE UGA_ManaPlayerZipToPoint* SetZipToPointAbility(UGA_ManaPlayerZipToPoint* ZipAbility) { return ActiveZipAbility = ZipAbility; }
	FORCEINLINE UGA_ManaPlayerSwing* SetSwingAbility(UGA_ManaPlayerSwing* SwingAbility) { return ActiveSwingAbility = SwingAbility; }
};
