// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_RandomMoveSpeed.h"

#include "Actors/BaseManaEnemy.h"
#include "AI/AIC_NPC.h"

void UBTService_RandomMoveSpeed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	AAIC_NPC* AI = Cast<AAIC_NPC>(OwnerComp.GetAIOwner());

	if (ABaseManaEnemy* Enemy = Cast<ABaseManaEnemy>(AI->GetPawn()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Set new speed here?");
	}
}
