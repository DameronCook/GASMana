#include "Actors/BaseManaEnemy.h"
#include "PlayerManaCharacter.h"
#include "Actors/SkyBeacon.h"
#include "AI/AIC_NPC.h"
#include "AI/ManaEnemyAnimInstance.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/SplineComponent.h"
#include "Components/WidgetComponent.h"
#include "Item/Equipment.h"
#include "Item/RightHandEquipment.h"
#include "Item/LeftHandEquipment.h"
#include "Item/ManaPickUp.h"
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

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ABaseManaEnemy::UnloadMeOnRestart);

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

void ABaseManaEnemy::UnloadMeOnRestart(UWorld* LoadedWorld)
{
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
	}}

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

	if (GetRightHandEquipment()) DropRightEquipment(GetRightHandEquipment());
	if (GetLeftHandEquipment()) DropLeftEquipment(GetLeftHandEquipment());

	DeathDrops();

	if (SkyBeacon)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Found sky beacon");
		if (SkyBeacon->GetClass()->ImplementsInterface(UEnemyList::StaticClass()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Sky beacon implements interface");
			IEnemyList::Execute_RemoveEnemyFromList(SkyBeacon, this);
		}
	}
}

void ABaseManaEnemy::DeathDrops()
{
	const int NumberOfDrops = UKismetMathLibrary::RandomIntegerInRange(1,3);
	for (int i = 0; i < NumberOfDrops; i++)
	{
		const FVector ActorLocation = GetActorLocation();
		const float RandX = UKismetMathLibrary::RandomFloatInRange(-150.f, 150.f);
		const float RandY = UKismetMathLibrary::RandomFloatInRange(-150.f, 150.f);
		const float RandZ = UKismetMathLibrary::RandomFloatInRange(0.f, 150.f);
		FVector SpawnLocation = FVector(ActorLocation.X + RandX, ActorLocation.Y + RandY, ActorLocation.Z + RandZ);
		FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation);
		AItem* Drop = GetWorld()->SpawnActorDeferred<AItem>(ItemDropClass, SpawnTransform);

		const int32 RandomIndex = FMath::RandRange(0, ItemRows.Num() - 1);

		const FPlayerItemSlot DroppedItemSlot = ItemRows[RandomIndex];

		if (Drop)
		{
			Drop->SetItemData(DroppedItemSlot);
			Drop->SetItem();
			Drop->FinishSpawning(SpawnTransform);
		}
	}
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