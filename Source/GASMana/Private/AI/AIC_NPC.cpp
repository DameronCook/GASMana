// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIC_NPC.h"

#include "BehaviorTree/BlackboardComponent.h"


AAIC_NPC::AAIC_NPC()
{
	BlackboardAsset = NewObject<UBlackboardData>();
	
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
}

void AAIC_NPC::BeginPlay()
{
	Super::BeginPlay();
	
	if (BlackboardAsset) UseBlackboard(BlackboardAsset, BlackboardComp);

	if (BlackboardComp) BlackboardComp->SetValueAsBool("IsRanged", bIsRanged);

	RunBehaviorTree(BehaviorTree);
}