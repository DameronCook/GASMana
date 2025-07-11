// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_WallRun.generated.h"

class UCharacterMovementComponent;
class APlayerManaCharacter;
class UGA_ManaPlayerWallRun;
/**
 *
 */
UENUM(BlueprintType)
enum class EWallRunSide : uint8
{
	Left  UMETA(DisplayName = "Left"),
	Right UMETA(DisplayName = "Right")
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GASMANA_API UAC_WallRun : public UActorComponent
{
	GENERATED_BODY()


	UAC_WallRun();

	/** Wall Jump MovementCurve */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Curves", meta = (AllowPrivateAccess = "true"))
	UCurveFloat* WallJumpCurveFloat;

	////////////////////////////// -- Montages -- \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

	/** Wall Run Right Montage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* WallRunLeftMontage;

	/** Wall Jump Montage */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* WallJumpMontage;

	/** Wall Run left Montage*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* WallRunRightMontage;

	/** Wall Run Montage To Play */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* PlayableWallRunMontage;


	//////////////////////// -- Wall Run Variables -- \\\\\\\\\\\\\\\\\\\\\\\\\\\\


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
	FVector UpdateWallRunHorizontal(APlayerManaCharacter* Character, UGA_ManaPlayerWallRun* WallRunAbility, UCharacterMovementComponent* CharMove);

	UFUNCTION()
	void UpdateWallRunVertical(float DeltaTime, APlayerManaCharacter* Character, UGA_ManaPlayerWallRun* WallRunAbility, UCharacterMovementComponent* CharMove);

	UFUNCTION()
	bool WallRunCheck(ACharacter* Character, UCharacterMovementComponent* CharMove);

	UFUNCTION()
	bool CanWallRunOnSurface(FVector ImpactNormal, UCharacterMovementComponent* CharMove);

	UFUNCTION()
	bool IsWallRunningAlongRightSide(FVector ImpactNormal, ACharacter* Character);

	UFUNCTION()
	bool ForwardWallRunCheck(ACharacter* Character);

	UFUNCTION()
	FVector SetWallRunDirection(FVector SideMultiplier, FVector ImpactNormal);


protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FORCEINLINE UAnimMontage* GetWallRunMontage() const { return PlayableWallRunMontage; }
	FORCEINLINE UAnimMontage* GetWallJumpMontage() const { return WallJumpMontage; }
	FORCEINLINE UCurveFloat* GetWallJumpCurveFloat() const { return WallJumpCurveFloat; }
	FORCEINLINE FVector GetWallRunDirection() const { return WallRunDirection; }
	FORCEINLINE FVector GetWallRunImpactNormal() const { return WallRunImpactNormal; }
	FORCEINLINE EWallRunSide GetWallRunSide() { return WallRunSide; }
};
