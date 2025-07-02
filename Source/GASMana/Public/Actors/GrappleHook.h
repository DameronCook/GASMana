// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ManaHookParent.h"
#include "GrappleHook.generated.h"

UCLASS()
class GASMANA_API AGrappleHook : public AActor
{
	GENERATED_BODY()
	
public:	
	AGrappleHook();

	virtual void Tick(float DeltaTime) override;

	float DistanceToTarget();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* HookMesh;

	UPROPERTY(VisibleAnywhere, Category = Movement)
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(BlueprintReadWrite, Category = Hooks, meta = (ExposeOnSpawn = "true"))
	AManaHookParent* TargetHook;

	float GrapplingSpeed;


protected:
	virtual void BeginPlay() override;

private:	
	FVector StartingLocation;
	FVector EndingLocation;
	FVector Direction;
};
