// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AC_HookShot.generated.h"

class AManaHookParent;
class APlayerManaCharacter;
class AGrappleHook;
class ACableActor;

UENUM(BlueprintType)
enum class EGrappleState : uint8
{
	E_Inactive  UMETA(DisplayName = "Inactive"),
	E_Firing UMETA(DisplayName = "Firing"),
	E_NearTarget UMETA(DisplayName = "NearTarget"),
	E_ZipToPointTarget UMETA(DisplayName = "ZipToPoint"),
	E_SwingTarget UMETA(DisplayName = "Swing"),

};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GASMANA_API UAC_HookShot : public UActorComponent
{
	GENERATED_BODY()

	EGrappleState GrappleState;

	UFUNCTION()
	void Inactive();

	void FindAndSetBestTarget(TArray<AActor*, FDefaultAllocator>& OutActors, APlayerManaCharacter* PlayerCharacter);

	UFUNCTION()
	void Firing();

	UFUNCTION()
	void NearTarget();

	UFUNCTION()
	void ZipToPointTarget(float DeltaTime);

	UFUNCTION()
	void SwingTarget(float DeltaTime);

	void PushForceAwayFromWalls(APlayerManaCharacter* PlayerCharacter, FVector PushAwayDirection);

	UPROPERTY()
	float MaxGrappleDistance;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AManaHookParent> ParentHook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	AGrappleHook* GrappleHook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AGrappleHook> GrappleHookClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	ACableActor* GrappleCable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> GrappleCableClass;

	float StartJumpDistance = 500.f;
	float EndJumpDistance = 150.f;
	bool HitTarget = false;
	FVector CharacterInitDirection = FVector::ZeroVector;


	AManaHookParent* CurrentTarget;

	FName SocketNameStart = "hand_rSocket";
	FName EndComponentName = "DefaultSceneRoot";

	bool DrawLineToTarget(ACharacter* Character, AActor* OverlappedActor);
	float CalculateAngleToTarget(ACharacter* Character, AActor* OverlappedActor);
	void SetCurrentTarget(AManaHookParent* Hook);


/// //////////////////////////////////////////
/// 	Swing Hook Variables & Functions

	FVector OptimalSwingPoint;
	FVector SwingTargetLocation;
	float SwingAngle;

	UFUNCTION()
	FVector FindOptimalSwingPoint(APlayerManaCharacter* Character);

	UFUNCTION()
	FVector FindSwingArcForce(APlayerManaCharacter* Character, float MinVelocity, float MaxVelocity, float ReduceSwingForceFactor);


	UFUNCTION()
	float FindSwingAngle(APlayerManaCharacter* Character);

	UFUNCTION()
	FRotator FindCharacterRotation(APlayerManaCharacter* Character, float DeltaTime);

	UFUNCTION()
	FRotator FindSwingSideAngle(APlayerManaCharacter* Character);


public:	
	UAC_HookShot();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AttemptGrapple();

	void EndGrapple();

	UFUNCTION()
	FVector FindSwingLaunchForce(APlayerManaCharacter* Character, float ForwardSpeed, float UpSpeed);

	FORCEINLINE class AManaHookParent* GetCurrentTarget() { return CurrentTarget; }
	FORCEINLINE FVector GetCharacterInitDir() { return CharacterInitDirection; }

protected:
	virtual void BeginPlay() override;
};
