// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseManaEnemy.h"
#include "Components/WidgetComponent.h"
#include "Item/Equipment.h"
#include "Item/RightHandEquipment.h"
#include "Item/LeftHandEquipment.h"
#include "UI/CameraTarget.h"

ABaseManaEnemy::ABaseManaEnemy()
{
	TargetedWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TargetedWidget"));
	TargetedWidget->SetWidgetClass(CameraTarget);
	TargetedWidget->SetupAttachment(RootComponent);
	
}

AEquipment* ABaseManaEnemy::EnemyEquip()
{
	if (!RightHandEquipment)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Running Grab Overlapping Item");
		GrabOverlappingItem();
	}

	if (!OverlappingItem)
	{
		if (RightHandEquipment || LeftHandEquipment)
		{
			GetAbilitySystemComponent()->TryActivateAbilitiesByTag(EquipTagContainer, true);
		}
	}
	
	if (RightHandEquipment)
	{
		return RightHandEquipment;
	}
	if (LeftHandEquipment) return LeftHandEquipment;
	return nullptr;
}

void ABaseManaEnemy::BeginPlay()
{
	Super::BeginPlay();

	// if (UWorld* World = GetWorld())
	// {
	// 	RightHandEquipment = World->SpawnActor<ARightHandEquipment>(RightHandEquipmentClass, GetActorTransform());
	// 	if (RightHandEquipment)
	// 	{
	// 		SetEquipment(RightHandEquipment);
	// 	}
	// }

	SetTargetWidgetIcon(false);

}


void ABaseManaEnemy::ShowHealth()
{
	//if (GEngine)
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, FString::Printf(TEXT("HEALTH: %f"), GetAbilitySystemComponent()->GetNumericAttribute(UManaAttributeSet::GetHealthAttribute())));
}

bool ABaseManaEnemy::DoMeleeAttack()
{
	return true;
}

void ABaseManaEnemy::SetTargetWidgetIcon(const bool IsTargeted) const
{
	if (TargetedWidget)
	{
		if (UCameraTarget* CamTarget = Cast<UCameraTarget>(TargetedWidget->GetUserWidgetObject()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "SetTargetBrush called!");
			if (IsTargeted) CamTarget->SetTargetBrush(TargetedTexture); else CamTarget->SetTargetBrush(NotTargetedTexture);
		}
	}
}