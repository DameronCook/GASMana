// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "TreasureParent.generated.h"

class UGameplayEffect;
class UNiagaraSystem;
class UNiagaraComponent;


USTRUCT(BlueprintType)
struct FTreasureParent : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Data")
	UDataTable* TreasureDataTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName ItemSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float TAmount;
};

UCLASS()
class GASMANA_API ATreasure : public AItem
{
	GENERATED_BODY()

	ATreasure();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess= "true"))
	UDataTable* TreasureDataTable;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FName ItemSize;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float TreasureAmount;
	
public:

	virtual void BeginPlay() override;

	virtual void SetItem() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult) override;


};
