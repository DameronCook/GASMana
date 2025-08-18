// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/SkeletalMesh.h"
#include "ManaEquipmentParent.generated.h"

UCLASS()
class GASMANA_API AManaEquipmentParent : public AActor
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* SkeletalMesh;
	
public:	
	AManaEquipmentParent();

	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

};
