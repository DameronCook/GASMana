#include "Actors/BaseManaEnemy.h"
#include "PlayerManaCharacter.h"
#include "AI/AIC_NPC.h"
#include "AI/ManaEnemyAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"
#include "Components/WidgetComponent.h"
#include "Item/Equipment.h"
#include "Item/RightHandEquipment.h"
#include "Item/LeftHandEquipment.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/CameraTarget.h"

ABaseManaEnemy::ABaseManaEnemy()
{
	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComponent"));
	SplineComponent->SetupAttachment(RootComponent);
	SplineComponent->bInputSplinePointsToConstructionScript = true;
	
	TargetedWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TargetedWidget"));
	TargetedWidget->SetWidgetClass(CameraTarget);
	TargetedWidget->SetupAttachment(RootComponent);
}

void ABaseManaEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (SplineComponent)
	{
		SplineCount = SplineComponent->GetNumberOfSplinePoints();

		for (int i = 0; i < SplineCount; i++)
		{
			PatrolPoints.Add(SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World));
			SplineComponent->SetSplinePointType(i, ESplinePointType::Linear);
		}
	}

	EnemyController = Cast<AAIC_NPC>(GetController());

	if (EnemyController)
	{
		if (UBlackboardComponent* BlackboardComponent = EnemyController->GetBlackboardComponent())
		{
			const uint8 Byte = static_cast<uint8>(PatrolType);
			BlackboardComponent->SetValueAsEnum("EnemyPatrolType", Byte);

			if (ShouldIBeInitiallyUnloaded)
			{
				UnloadMe();
				BlackboardComponent->SetValueAsBool("AmILoaded", false);
			}
			else
			{
				BlackboardComponent->SetValueAsBool("AmILoaded", true);
			}
		}
	}

	SetTargetWidgetIcon(false, this);
}

void ABaseManaEnemy::IterateNextPoint()
{
	int Iterator = -1;
	if (PatrolDir) Iterator = 1;
	PatrolIndex += Iterator;
}

FVector ABaseManaEnemy::GetNextPatrolPoint()
{
	const int NumOfPoints = PatrolPoints.Num() - 1;
	if (NumOfPoints == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Patrol points set!!!"));
		return GetActorLocation();
	}

	PatrolIndex += (PatrolDir ? 1 : -1);
	
	if (PatrolIndex < 0 || PatrolIndex >= NumOfPoints)
	{
		if (SplineComponent->IsClosedLoop())
		{
			PatrolIndex = PatrolDir ? 0 : NumOfPoints;
		}
		else
		{
			PatrolDir = !PatrolDir;
			PatrolIndex += (PatrolDir ? 1 : NumOfPoints - 1);
		}
	}

	PatrolIndex = FMath::Clamp(PatrolIndex, 0, NumOfPoints);
	UE_LOG(LogTemp, Warning, TEXT("Next Patrol Point: %s"), *PatrolPoints[PatrolIndex].ToString());
	return PatrolPoints[PatrolIndex];
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
	if (EnemyController) EnemyController->ActivateTree();
}

void ABaseManaEnemy::UnloadMe()
{
	Super::UnloadMe();
	SetActorHiddenInGame(true);
	SetActorTickEnabled(false);
	if (UBlackboardComponent* BlackboardComponent = EnemyController->GetBlackboardComponent()) BlackboardComponent->SetValueAsBool("AmILoaded", false);
}

void ABaseManaEnemy::DirectionalHitReact(const FVector& HitterLocation, bool IsFinisher)
{
	Super::DirectionalHitReact(HitterLocation, IsFinisher);

	/*
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), HitterLocation));
	
	if (EnemyController)
	{
		if (UBlackboardComponent* BlackboardComponent = EnemyController->GetBlackboardComponent())
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Got controller and blackboard component!");

			if (EnemyController->GetSensedActor())
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Setting target to follow!");

				BlackboardComponent->SetValueAsObject("TargetToFollow", EnemyController->GetSensedActor());
			}
		}
	}
	*/
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

void ABaseManaEnemy::Die(const FVector& HitLocation)
{
	Super::Die(HitLocation);

	PlayerCharacterDieMontage(HitLocation);

	if (EnemyController)
	{
		if (UBlackboardComponent* BlackboardComponent = EnemyController->GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsBool("IsAlive", false);
		}
	}

	GetTargetedWidget()->Deactivate();
	GetTargetedWidget()->SetVisibility(false);
	
	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(DeathTagContainer, true);
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
	if (TargetedWidget)
	{
		if (UCameraTarget* CamTarget = Cast<UCameraTarget>(TargetedWidget->GetUserWidgetObject()))
		{
			if (IsTargeted) CamTarget->SetTargetBrush(TargetedTexture); else CamTarget->SetTargetBrush(NotTargetedTexture);
		}
	}
}