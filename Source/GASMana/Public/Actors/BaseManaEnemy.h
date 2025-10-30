// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../GASManaCharacter.h"
#include "BaseManaEnemy.generated.h"

class AAIC_NPC;
class USplineComponent;
class UImage;
class UCameraTarget;
class UWidgetComponent;

/**
 * 
 */
UCLASS()
class GASMANA_API ABaseManaEnemy : public AGASManaCharacter
{
	GENERATED_BODY()
	virtual void BeginPlay() override;
	virtual void GetMontageToPlay() override;
	
	ABaseManaEnemy();

	UFUNCTION()
	void IterateNextPoint();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Patrol, meta = (AllowPrivateAccess = "true"))
	int SplineCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Patrol, meta = (AllowPrivateAccess = "true"))
	TArray<FVector> PatrolPoints;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Patrol, meta = (AllowPrivateAccess = "true"))
	int PatrolIndex = 0;
	/* Flip this to change the direction of patrol */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Patrol, meta = (AllowPrivateAccess = "true"))
	bool PatrolDir;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Loading, meta = (AllowPrivateAccess = "true"))
	bool ShouldIBeInitiallyUnloaded = true;
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ARightHandEquipment> RightHandEquipmentClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	USplineComponent* SplineComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* TargetedWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraTarget> CameraTarget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Widget)
	FSlateBrush TargetedTexture;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Widget)
	FSlateBrush NotTargetedTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Patrol, meta = (AllowPrivateAccess = "true"))
	EPatrolType PatrolType;

	UPROPERTY()
	AAIC_NPC* EnemyController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Load)
	USphereComponent* LoadSphere;

public:
	UFUNCTION()
	static void ShowHealth();

	UFUNCTION()
	static bool DoMeleeAttack();

	UFUNCTION()
	void SetTargetWidgetIcon(bool IsTargeted, const AActor* Caller) const;

	FORCEINLINE UWidgetComponent* GetTargetedWidget() const { return TargetedWidget; }

	/**Called by tree to grab nearby weapon */
	UFUNCTION(Category = "Combat")
	AEquipment* EnemyEquip();

	UFUNCTION(Category = "Loading")
	virtual void LoadMe() override;

	UFUNCTION(Category = "Loading")
	virtual void UnloadMe() override;

	UFUNCTION()
	FVector GetNextPatrolPoint();
};

