// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DT_TreasurePickups.generated.h"


USTRUCT(BlueprintType)
struct GASMANA_API FTreasureDT : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemSize;
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TAmount;

	
	
};



class UGameplayEffect;
class UNiagaraSystem;
class UNiagaraComponent;




