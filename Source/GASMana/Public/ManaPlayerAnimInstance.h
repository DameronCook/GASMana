// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Character/CharacterTypes.h"
#include "ManaPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UManaPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "Lean")
	FVector CalculateRelativeAccelerationAmount();

	UFUNCTION(BlueprintCallable, Category = "Velocity")
	FVector GetVelocityAccel();

	FVector VelocityLastFrame;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blocking")
	bool bIsBlocking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Atacking")
	bool bIsAttacking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Moving")
	bool bIsRunning = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Moving")
	bool bIsEquipping = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blocking")
	float GroundSpeed = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blocking")
	bool isFalling = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blocking")
	float LeanAmount = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blocking")
	EEquipmentState EES;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaTime) override;

	UPROPERTY(BlueprintReadOnly)
	class APlayerManaCharacter* ManaCharacter;

	UPROPERTY(BlueprintReadOnly)
	class UCharacterMovementComponent* ManaMovementComponent;


	UFUNCTION(BlueprintCallable, Category = "Bools")
	void SetIsBlocking(bool bBlocking);
	
	UFUNCTION(BlueprintCallable, Category = "Bools")
	void SetIsAttacking(bool bAttacking);
	
	UFUNCTION(BlueprintCallable, Category = "Bools")
	void SetIsRunning(bool bRunning);
	
	UFUNCTION(BlueprintCallable, Category = "Bools")
	void SetIsEquipping(bool bEquipping);
	
	UFUNCTION(BlueprintCallable, Category = "Lean")
	float Get_LeanAmount();	
};
