// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BaseManaEnemy.h"
#include "ManaAttributeSet.h"
#include "Item/RightHandEquipment.h"

void ABaseManaEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		RightHandEquipment = World->SpawnActor<ARightHandEquipment>(RightHandEquipmentClass, GetActorTransform());
		if (RightHandEquipment)
		{
			SetEquipment(RightHandEquipment);
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
