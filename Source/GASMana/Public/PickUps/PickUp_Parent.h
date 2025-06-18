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

	void OnPickedUp_Implementation(AActor* Picker, UAbilitySystemComponent* AbilitySystem);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSubclassOf<UGameplayEffect> PickUpEffect;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	class UCapsuleComponent* CapsuleComponent;
};
