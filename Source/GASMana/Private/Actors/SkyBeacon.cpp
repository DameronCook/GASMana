// Fill out your copyright notice in the Description page of Project Settings.
#include "Actors/SkyBeacon.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
ASkyBeacon::ASkyBeacon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StartBeacon();
}

void ASkyBeacon::StartBeacon()
{
	if (!BeaconNiagaraComponent)
	{
		BeaconNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BeaconNiagaraSystem, GetActorLocation(), FRotator::ZeroRotator);
	}
	if (BeaconNiagaraComponent) BeaconNiagaraComponent->Activate();
}

void ASkyBeacon::EndBeacon() const
{
	if (BeaconNiagaraComponent)
	{
		BeaconNiagaraComponent->Deactivate();
	}
}

void ASkyBeacon::CheckEnemiesLeft()
{
	if (EnemiesToClear.Num() <= 0)
	{
		EndBeacon();
	}
}

void ASkyBeacon::BeginPlay()
{
	Super::BeginPlay();

	StartBeacon();
}

void ASkyBeacon::RemoveEnemyFromList_Implementation(ABaseManaEnemy* Enemy)
{
	//IEnemyList::RemoveEnemyFromList(Enemy);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "End Beacon has been called?");
	
	EnemiesToClear.Remove(Enemy);

	CheckEnemiesLeft();
}
