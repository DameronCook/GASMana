// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../GASManaCharacter.h"
#include "BaseManaEnemy.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API ABaseManaEnemy : public AGASManaCharacter
{
	GENERATED_BODY()
	virtual void BeginPlay() override;
	
public:
	UFUNCTION()
	static void ShowHealth();

	UFUNCTION()
	static bool DoMeleeAttack();

	UPROPERTY(EditAnywhere)
	TSubclassOf<ARightHandEquipment> RightHandEquipmentClass;
};
