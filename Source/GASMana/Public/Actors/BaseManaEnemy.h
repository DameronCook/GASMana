// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../../GASManaCharacter.h"
#include "BaseManaEnemy.generated.h"

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

	ABaseManaEnemy();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ARightHandEquipment> RightHandEquipmentClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* TargetedWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Widget, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UCameraTarget> CameraTarget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Widget)
	FSlateBrush TargetedTexture;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Widget)
	FSlateBrush NotTargetedTexture;
	
public:
	UFUNCTION()
	static void ShowHealth();

	UFUNCTION()
	static bool DoMeleeAttack();

	UFUNCTION()
	void SetTargetWidgetIcon(bool IsTargeted) const;

	FORCEINLINE UWidgetComponent* GetTargetedWidget() const { return TargetedWidget; }

};
