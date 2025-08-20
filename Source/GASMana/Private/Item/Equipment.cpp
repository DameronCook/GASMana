// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipment.h"


// Sets default values
AEquipment::AEquipment()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);
}

void AEquipment::AttachMeshToSocket(USceneComponent* InParent, FName InSocketName) const
{
	const FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	SkeletalMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}
