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

	/** Number of seconds hitstop will last before resuming gameplay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hookshot", meta = (AllowPrivateAccess = "true"))
	float HitStopDuration;

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

	void PushForceAwayFromWalls(APlayerManaCharacter* PlayerCharacter, const FVector& PushAwayDirection) const;

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

	bool DrawLineToTarget(const ACharacter* Character, const AActor* OverlappedActor) const;
	static float CalculateAngleToTarget(ACharacter* Character, const AActor* OverlappedActor);
	void SetCurrentTarget(AManaHookParent* Hook);


/// //////////////////////////////////////////
/// 	Swing Hook Variables & Functions

	FVector OptimalSwingPoint;
	FVector SwingTargetLocation;
	float SwingAngle;

	UFUNCTION()
	static FVector FindOptimalSwingPoint(const APlayerManaCharacter* Character);

	UFUNCTION()
	FVector FindSwingArcForce(const APlayerManaCharacter* Character, float MinVelocity, float MaxVelocity, float ReduceSwingForceFactor) const;


	UFUNCTION()
	float FindSwingAngle(const APlayerManaCharacter* Character) const;

	UFUNCTION()
	FRotator FindCharacterRotation(const APlayerManaCharacter* Character, float DeltaTime) const;

	UFUNCTION()
	FRotator FindSwingSideAngle(const APlayerManaCharacter* Character) const;


public:	
	UAC_HookShot();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AttemptGrapple();

	void EndGrapple();

	UFUNCTION()
	FVector FindSwingLaunchForce(const APlayerManaCharacter* Character, float ForwardSpeed, float UpSpeed) const;

	FORCEINLINE class AManaHookParent* GetCurrentTarget() const { return CurrentTarget; }
	FORCEINLINE FVector GetCharacterInitDir() const { return CharacterInitDirection; }

protected:
	virtual void BeginPlay() override;
};
