// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ManaPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UManaPlayerAnimInstance : public UAnimInstance
{
	GENERATED_BODY()


protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blocking")
	bool bIsBlocking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Atacking")
	bool bIsAttacking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Moving")
	bool bIsRunning = false;

public:
	UFUNCTION(BlueprintCallable, Category = "Blocking")
	void SetIsBlocking(bool bBlocking);
	
	UFUNCTION(BlueprintCallable, Category = "Blocking")
	void SetIsAttacking(bool bAttacking);
	
	UFUNCTION(BlueprintCallable, Category = "Blocking")
	void SetIsRunning(bool bRunning);
	
};
