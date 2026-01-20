// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equipment.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

// Sets default values
AEquipment::AEquipment()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(RootComponent);

	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliSource"));

}

void AEquipment::BeginPlay()
{
	Super::BeginPlay();
	if (StimuliSource) StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
}

void AEquipment::AttachMeshToSocket(USceneComponent* InParent, const FName InSocketName) const
{
	const FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	SkeletalMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AEquipment::BreakEquipment()
{
	Destroy();
}

void AEquipment::SetCurDurability(const int NewDurability)
{
	CurDurability = NewDurability;
	if (CurDurability <= 0)
	{
		BreakEquipment();
	}
}
