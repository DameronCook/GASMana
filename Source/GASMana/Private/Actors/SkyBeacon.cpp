// Fill out your copyright notice in the Description page of Project Settings.
#include "Actors/SkyBeacon.h"

#include "ManaPlayerController.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "PlayerManaCharacter.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UI/EnemyCounter.h"

// Sets default values
ASkyBeacon::ASkyBeacon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StartBeacon();

	//Create a sphere mesh that checks for enemies to load them in
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	SphereComponent->SetupAttachment(RootComponent);
	SphereComponent->SetSphereRadius(6000.f, false);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void ASkyBeacon::StartBeacon()
{
	if (!BeaconNiagaraComponent)
	{
		BeaconNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BeaconNiagaraSystem, GetActorLocation(), FRotator::ZeroRotator);
	}
	if (BeaconNiagaraComponent) BeaconNiagaraComponent->Activate();
}

void ASkyBeacon::EndBeacon()
{
	if (APlayerManaCharacter* PlayerChar = Cast<APlayerManaCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
	{
		PlayerChar->RemoveBeaconFromList(this);
	}
	
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

		if (EnemyCounter)
		{
			EnemyCounter->RemoveFromParent();
			EnemyCounter = nullptr;
		}
	}
}

void ASkyBeacon::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (APlayerManaCharacter* PlayerChar = Cast<APlayerManaCharacter>(OtherActor))
	{
		if (AManaPlayerController* Controller = Cast<AManaPlayerController>(PlayerChar->GetController()))
		{
			EnemyCounter = Controller->GetEnemyCounter();
			EnemyCounter->AddToViewport();
			EnemyCounter->SetCounterText(FText::FromString(FString::Printf(TEXT("Enemies to clear: %d / %d"), EnemiesToClear.Num(), InitialCount)));
			
		}
	}
}

void ASkyBeacon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (EnemyCounter)
	{
		EnemyCounter->RemoveFromParent();
		EnemyCounter = nullptr;	
	}
}

void ASkyBeacon::BeginPlay()
{
	Super::BeginPlay();

	StartBeacon();
	
	if (APlayerManaCharacter* PlayerChar = Cast<APlayerManaCharacter>(UGameplayStatics::GetPlayerPawn(this, 0)))
	{
		PlayerChar->Beacons.Add(this);
	}

	if (SphereComponent)
	{
		SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ASkyBeacon::OnSphereOverlap);
		SphereComponent->OnComponentEndOverlap.AddDynamic(this, &ASkyBeacon::OnSphereEndOverlap);
	}

	InitialCount = EnemiesToClear.Num();

}

void ASkyBeacon::RemoveEnemyFromList_Implementation(ABaseManaEnemy* Enemy)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "End Beacon has been called?");
	
	EnemiesToClear.Remove(Enemy);

	EnemyCounter->SetCounterText(FText::FromString(FString::Printf(TEXT("Enemies to clear: %d / %d"), EnemiesToClear.Num(), InitialCount)));

	CheckEnemiesLeft();
}
