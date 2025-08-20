// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interface/I_PickUpInterface.h"
#include "GameplayEffect.h"
#include "PickUp_Parent.generated.h"

UCLASS()
class GASMANA_API APickUp_Parent : public AActor, public II_PickUpInterface
{
	GENERATED_BODY()
	
public:	
	APickUp_Parent();

	virtual void OnPickedUp_Implementation(AActor* Picker, UAbilitySystemComponent* AbilitySystem) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSubclassOf<UGameplayEffect> PickUpEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UCapsuleComponent* CapsuleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash")
	FVector FlashColor = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash")
	FName FlashParamName = "EffectColor";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash")
	FName FlashLengthName = "EffectLength";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash")
	float FlashLength = 1.f;
};
