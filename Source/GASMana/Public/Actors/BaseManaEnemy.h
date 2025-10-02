// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../GASManaCharacter.h"
#include "BaseManaEnemy.generated.h"

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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ARightHandEquipment> RightHandEquipmentClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	USplineComponent* SplineComponent;

	UPROPERTY()
	int SplineCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* TargetedWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraTarget> CameraTarget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Widget)
	FSlateBrush TargetedTexture;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Widget)
	FSlateBrush NotTargetedTexture;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EPatrolType PatrolType;
	
public:
	UFUNCTION()
	static void ShowHealth();

	UFUNCTION()
	static bool DoMeleeAttack();

	UFUNCTION()
	void SetTargetWidgetIcon(bool IsTargeted) const;

	FORCEINLINE UWidgetComponent* GetTargetedWidget() const { return TargetedWidget; }

	/**Called by tree to grab nearby weapon */
	UFUNCTION(Category = "Combat")
	AEquipment* EnemyEquip();

};

