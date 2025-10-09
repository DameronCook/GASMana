// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseManaEnemy.h"

#include "PlayerManaCharacter.h"
#include "AI/AIC_NPC.h"
#include "AI/ManaEnemyAnimInstance.h"
#include "Components/SphereComponent.h"
#include "Components/SplineComponent.h"
#include "Components/WidgetComponent.h"
#include "Item/Equipment.h"
#include "Item/RightHandEquipment.h"
#include "Item/LeftHandEquipment.h"
#include "UI/CameraTarget.h"

ABaseManaEnemy::ABaseManaEnemy()
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(RootComponent);
	SplineCount = SplineComponent->GetNumberOfSplinePoints();
	
	TargetedWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TargetedWidget"));
	TargetedWidget->SetWidgetClass(CameraTarget);
	TargetedWidget->SetupAttachment(RootComponent);
}

AEquipment* ABaseManaEnemy::EnemyEquip()
{
	AItem* TempItem = OverlappingItem;
	
	if (!RightHandEquipment || !LeftHandEquipment)
	{
		GrabOverlappingItem();
	}

	if (!OverlappingItem)
	{
		if (RightHandEquipment || LeftHandEquipment)
		{
			if (GetAbilitySystemComponent()->TryActivateAbilitiesByTag(EquipTagContainer, true))
			{
				if (UManaEnemyAnimInstance* AnimInstance = Cast<UManaEnemyAnimInstance>(GetMesh()->GetAnimInstance())) AnimInstance->SetIsEquipping(true);
				return Cast<AEquipment>(TempItem);
			}
		}
	}
	
	return nullptr;
}

void ABaseManaEnemy::LoadMe()
{
	Super::LoadMe();

	SetActorHiddenInGame(false);
	SetActorTickEnabled(true);
	if (AAIC_NPC* ActorController = Cast<AAIC_NPC>(Controller))
	{
		ActorController->ActivateTree();
	}
}

void ABaseManaEnemy::UnloadMe()
{
	Super::UnloadMe();
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	//Maybe stop tree here?
}


void ABaseManaEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	EnemyController = Cast<AAIC_NPC>(GetController());

	if (UCameraTarget* CamTarget = Cast<UCameraTarget>(TargetedWidget->GetUserWidgetObject()))
	{
		CamTarget->SetTargetBrush(EmptyTexture);
	}

	UnloadMe();
}

void ABaseManaEnemy::GetMontageToPlay()
{
	UAnimMontage* MontageToPlay;

	if (EquipmentState == EEquipmentState::EES_Unequipped)
	{
		//GEngine->AddOnScreenDebugMessage(-1,5.f, FColor::Purple, "Getting Equip Attack!");
		MontageToPlay = RightHandEquipment->GetEquipAttack();
		RemoveFreeTag();
	}
	else
	{
		//If we're not moving
		RemoveFreeTag();
		MontageToPlay = RightHandEquipment->GetAttackCombo();
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Montage set to: %s"), *MontageToPlay->GetName()));
	if (MontageToPlay) SetAttackMontage(MontageToPlay);	
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

void ABaseManaEnemy::SetTargetWidgetIcon(const bool IsTargeted, const AActor* Caller) const
{
	if (GetDistanceTo(Caller) >= 5000.f)
	{
		if (UCameraTarget* CamTarget = Cast<UCameraTarget>(TargetedWidget->GetUserWidgetObject()))
		{
			CamTarget->SetTargetBrush(EmptyTexture);
			return;
		}
	}
	if (TargetedWidget)
	{
		if (UCameraTarget* CamTarget = Cast<UCameraTarget>(TargetedWidget->GetUserWidgetObject()))
		{
			if (IsTargeted) CamTarget->SetTargetBrush(TargetedTexture); else CamTarget->SetTargetBrush(NotTargetedTexture);
		}
	}
}