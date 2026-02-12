// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/EnemyList.h"
#include "SkyBeacon.generated.h"

class ABaseManaEnemy;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class GASMANA_API ASkyBeacon : public AActor, public IEnemyList
{
	GENERATED_BODY()
	ASkyBeacon();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* BeaconNiagaraSystem;

	UPROPERTY()
	UNiagaraComponent* BeaconNiagaraComponent;

	UFUNCTION()
	void StartBeacon();
	UFUNCTION()
	void EndBeacon() const;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<ABaseManaEnemy*> EnemiesToClear;

	UFUNCTION()
	void CheckEnemiesLeft();
	
protected:
	virtual void BeginPlay() override;

public:
	virtual void RemoveEnemyFromList_Implementation(ABaseManaEnemy* Enemy) override;
};
