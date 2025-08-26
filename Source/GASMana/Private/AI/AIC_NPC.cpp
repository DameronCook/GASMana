// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIC_NPC.h"

#include "BehaviorTree/BlackboardComponent.h"

void AAIC_NPC::BeginPlay()
{
	Super::BeginPlay();

	UseBlackboard(BlackboardAsset, BlackboardComp);

	BlackboardComp->SetValueAsBool("IsRanged", bIsRanged);

	RunBehaviorTree(BehaviorTree);
}
