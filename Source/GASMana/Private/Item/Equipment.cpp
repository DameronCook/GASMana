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
	DurabilityFlashThreshold = FMath::CeilToInt(CurDurability/3.33);
	SetFresnelEmissive(1);

}

void AEquipment::AttachMeshToSocket(USceneComponent* InParent, const FName InSocketName) const
{
	const FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	SkeletalMesh->AttachToComponent(InParent, TransformRules, InSocketName);
	SetFresnelEmissive(0);
}

void AEquipment::DetachMeshFromSocket(const FDetachmentTransformRules Rules) const
{
	SkeletalMesh->DetachFromComponent(Rules);
	const FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	SkeletalMesh->AttachToComponent(RootComponent, TransformRules);
	SetFresnelEmissive(1);
}

void AEquipment::BreakEquipment()
{
	GetWorld()->GetTimerManager().ClearTimer(FlashTimerHandle);
	Destroy();
}

void AEquipment::RegisterAsSource()
{
	if (StimuliSource) StimuliSource->RegisterForSense(UAISense_Sight::StaticClass());
}

void AEquipment::SetCurDurability(const int NewDurability)
{
	CurDurability = NewDurability;

	if (CurDurability <= DurabilityFlashThreshold && !bAlreadyFlashing)
	{
		LowDurabilityFlashing();
	}
	
	if (CurDurability <= 0)
	{
		BreakEquipment();
	}
}

void AEquipment::LowDurabilityFlashing()
{
	PlayFlashEffect(FVector(1,0,0), 1);

	if (!bAlreadyFlashing)
	{
		bAlreadyFlashing = true;
		GetWorld()->GetTimerManager().SetTimer(FlashTimerHandle, this, &AEquipment::LowDurabilityFlashing, 2.f, true);
	}
}

void AEquipment::PlayFlashEffect(const FVector& InColor, const float FlashLength) const
{
	if (SkeletalMesh)
	{
		SkeletalMesh->SetVectorParameterValueOnMaterials("EffectColor", InColor);
		SkeletalMesh->SetScalarParameterValueOnMaterials("StartTime",
			GetWorld()->GetTimeSeconds());
		SkeletalMesh->SetScalarParameterValueOnMaterials("EffectLength", FlashLength);
	}
}

void AEquipment::SetFresnelEmissive(const float NewValue) const
{
	if (SkeletalMesh)
	{
		SkeletalMesh->SetScalarParameterValueOnMaterials("FresnelOn", NewValue);
	}
}
