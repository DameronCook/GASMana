// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AdvancedCameraComponent.generated.h"


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

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GASMANA_API UAdvancedCameraComponent : public UActorComponent
{
	GENERATED_BODY()

	bool ChangeCamera = false;

protected:
	virtual void BeginPlay() override;

	FCameraState CurrentCameraState;
	FCameraState TargetCameraState;
	float CameraInterpSpeed = 5.f;

public:	
	UAdvancedCameraComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	void UpdateCameraLengthRotationFOV(float DeltaTime);
	void SetCameraState(const FCameraState& NewCameraState, float InterpSpeed = 5.f);

		
};
