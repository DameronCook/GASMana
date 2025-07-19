// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AdvancedCameraComponent.generated.h"

class APlayerManaCharacter;
class UCameraComponent;
USTRUCT(BlueprintType)
struct FCameraState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TargetArmLength = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FRotator CameraRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float CameraFOV = 90.f;
};

UENUM(BlueprintType)
enum class ECameraState : uint8
{
	E_Default  UMETA(DisplayName = "Default"),
	E_ZipToPoint UMETA(DisplayName = "ZipToPoint"),
	E_Swing UMETA(DisplayName = "Swing"),
	E_WallRun UMETA(DisplayName = "WallRun"),
	E_WallJump UMETA(DisplayName = "WallJump"),
	E_Roll UMETA(DisplayName = "Roll"),
	E_Shield UMETA(DisplayName = "Shield")
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GASMANA_API UAdvancedCameraComponent : public UActorComponent
{
	GENERATED_BODY()

	bool ChangeCamera = false;


	UFUNCTION(Category = "Camera")
	void SetDefaultCameraState(APlayerManaCharacter* PlayerCharacter);

	UFUNCTION(Category = "Camera")
	void SetWallRunCameraState(UAC_WallRun* WallRun, APlayerManaCharacter* PlayerCharacter);

	UFUNCTION(Category = "Camera")
	void SetZipToPointCameraState(APlayerManaCharacter* PlayerCharacter);

	UFUNCTION(Category = "Camera")
	void SetSwingCameraState(APlayerManaCharacter* PlayerCharacter);

	UFUNCTION(Category = "Camera")
	void SetRollCameraState(APlayerManaCharacter* PlayerCharacter);

	UFUNCTION(Category = "Camera")
	void SetWallJumpCameraState(UAC_WallRun* WallRun, APlayerManaCharacter* PlayerCharacter);

	UFUNCTION(Category = "Camera")
	void SetShieldCameraState(APlayerManaCharacter* PlayerCharacter);

protected:
	virtual void BeginPlay() override;

	FCameraState CurrentCameraState;
	FCameraState TargetCameraState;
	float CameraInterpSpeed = 5.f;

public:	
	UAdvancedCameraComponent();

	UPROPERTY()
	ECameraState CamState = ECameraState::E_Default;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void UpdateCameraLengthRotationFOV(float DeltaTime, APlayerManaCharacter* PlayerCharacter);
	void SetCameraState(const FCameraState& NewCameraState, float InterpSpeed = 5.f);
	//void PushForceAwayFromWalls(FVector PushAwayDirection, APlayerManaCharacter* PlayerCharacter, UCameraComponent* Cam);
		
};
