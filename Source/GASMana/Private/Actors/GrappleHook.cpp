// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/GrappleHook.h"
#include "Actors/GrappleCable.h"
#include "GameFramework/ProjectileMovementComponent.h"
// Sets default values
AGrappleHook::AGrappleHook()
{
	PrimaryActorTick.bCanEverTick = true;

	HookMesh = CreateDefaultSubobject<UStaticMesh>(TEXT("Hook"));

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovementComponent->ProjectileGravityScale = 0.f;
}

void AGrappleHook::BeginPlay()
{
	Super::BeginPlay();

	StartingLocation = GetActorLocation();

	if (TargetHook)
	{
		
		EndingLocation = TargetHook->GetActorLocation();

		FVector LocDirection = EndingLocation - StartingLocation;

		Direction = LocDirection.GetSafeNormal();

		ProjectileMovementComponent->Velocity = Direction * GrapplingSpeed;
		
		//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, FString("So the begin play was run???"));
	}
}

void AGrappleHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (TargetHook)
	{
		float DistFromStart = FVector::DistSquared(GetActorLocation(), StartingLocation);
		float DistStartToTarget = FVector::DistSquared(TargetHook->GetActorLocation(), StartingLocation);

		if (DistFromStart >= DistStartToTarget)
		{
			float dist = FVector::Distance(StartingLocation, EndingLocation);

			FVector LengthToEnd = dist * Direction;

			FVector FinalLoc = LengthToEnd + StartingLocation;

			SetActorLocation(FinalLoc);
			ProjectileMovementComponent->Velocity = FVector::ZeroVector;
		}
	}
	
}

float AGrappleHook::DistanceToTarget()
{
	return FVector::Distance(EndingLocation, GetActorLocation());
}

