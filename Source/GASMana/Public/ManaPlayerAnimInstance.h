// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ManaCharacterAnimInstance.h"
#include "PlayerManaCharacter.h"
#include "Animation/AnimInstance.h"
#include "ManaPlayerAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UManaPlayerAnimInstance : public UManaCharacterAnimInstance
{
	GENERATED_BODY()
	virtual void NativeInitializeAnimation() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY()
	APlayerManaCharacter* ManaPlayer;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn")
	bool bShouldTurnInPlace;

public:
	FORCEINLINE bool SetShouldTurnInPlace(const bool TurnInPlace) { return bShouldTurnInPlace = TurnInPlace; }
};