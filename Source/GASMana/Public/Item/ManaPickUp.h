// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "ManaPickUp.generated.h"

class UGameplayEffect;
class UNiagaraSystem;
class UNiagaraComponent;


USTRUCT(BlueprintType)
struct FManaData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	TSubclassOf<UGameplayEffect> PickUpEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effect")
	UNiagaraSystem* ManaEffect;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash")
	FVector FlashColor = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash")
	FName FlashParamName = "EffectColor";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash")
	FName FlashLengthName = "EffectLength";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flash")
	float FlashLength = 1.f;
};

UCLASS()
class GASMANA_API AManaPickUp : public AItem
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UNiagaraSystem* ManaEffect;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	UNiagaraComponent* ManaComponent;
	
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UGameplayEffect> PickUpEffect;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FVector FlashColor = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FName FlashParamName = "EffectColor";

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	FName FlashLengthName = "EffectLength";

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	float FlashLength = 1.f;
	
public:
	
	AManaPickUp();

	virtual void BeginPlay() override;

	virtual void SetItem() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                             UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                             const FHitResult& SweepResult) override;

};
