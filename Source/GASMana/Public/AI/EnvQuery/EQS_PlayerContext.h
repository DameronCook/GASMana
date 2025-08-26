// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EQS_PlayerContext.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UEQS_PlayerContext : public UEnvQueryContext
{
	GENERATED_BODY()

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;

	UPROPERTY(EditAnywhere, Category = "EQS")
	TSubclassOf<AActor> PlayerClass;
};
