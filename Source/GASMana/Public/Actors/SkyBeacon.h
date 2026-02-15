// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/EnemyList.h"
#include "SkyBeacon.generated.h"

class USphereComponent;
class UEnemyCounter;
class ABaseManaEnemy;
class UNiagaraComponent;
class UNiagaraSystem;

UCLASS()
class GASMANA_API ASkyBeacon : public AActor, public IEnemyList
{
	GENERATED_BODY()
	ASkyBeacon();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* SphereComponent;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* BeaconNiagaraSystem;

	UPROPERTY()
	UNiagaraComponent* BeaconNiagaraComponent;

	UFUNCTION()
	void StartBeacon();

	UFUNCTION()
	void EndBeacon();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TArray<ABaseManaEnemy*> EnemiesToClear;

	UFUNCTION()
	void CheckEnemiesLeft();

	UPROPERTY()
	UEnemyCounter* EnemyCounter;

	int InitialCount;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
							   int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
protected:
	virtual void BeginPlay() override;

public:
	virtual void RemoveEnemyFromList_Implementation(ABaseManaEnemy* Enemy) override;
};
