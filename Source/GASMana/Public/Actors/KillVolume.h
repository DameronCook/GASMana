// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "KillVolume.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API AKillVolume : public ATriggerVolume
{
	GENERATED_BODY()

	AKillVolume();
	
	virtual void NotifyActorBeginOverlap(AActor* Other) override;

};
