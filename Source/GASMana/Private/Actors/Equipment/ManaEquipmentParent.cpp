// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Equipment/ManaEquipmentParent.h"

// Sets default values
AManaEquipmentParent::AManaEquipmentParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
}

void AManaEquipmentParent::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	SkeletalMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

