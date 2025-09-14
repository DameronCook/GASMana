// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIC_NPC.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Item/Item.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"


AAIC_NPC::AAIC_NPC()
{
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(FName("PerceptionComp"));
 
	//Create a Sight Sense
	Sight = CreateDefaultSubobject<UAISenseConfig_Sight>(FName("Sight Config"));
 
	Sight->SightRadius = 1000.f;
	Sight->LoseSightRadius = 1500.f;
	Sight->PeripheralVisionAngleDegrees = 130.f;
 
	//Tell the sight sense to detect everything
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = true;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;
 
	//Register the sight sense to our Perception Component
	AIPerceptionComponent->ConfigureSense(*Sight);
}

void AAIC_NPC::BeginPlay()
{
	Super::BeginPlay();
	
	if (BlackboardAsset) UseBlackboard(BlackboardAsset, BlackboardComp);

	if (BlackboardComp) BlackboardComp->SetValueAsBool("IsRanged", bIsRanged);

	RunBehaviorTree(BehaviorTree);
}

void AAIC_NPC::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	
	if (BlackboardAsset) UseBlackboard(BlackboardAsset, BlackboardComp);

	if (BlackboardComp) BlackboardComp->SetValueAsBool("IsRanged", bIsRanged);

	RunBehaviorTree(BehaviorTree);
}

AActor* AAIC_NPC::GetSeeingPawn() const
{
	
	//Return the seeing pawn
	UObject* Object = BlackboardComp->GetValueAsObject(BlackboardTargetKey);
 
	return Object ?  Cast<AActor>(Object) : nullptr;
}

void AAIC_NPC::OnSenseUpdated(const TArray<AActor*>& DetectedActors)
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Orange,"SenseUpdated!");
	/*
	//If our character exists inside the UpdatedActors array, register him
	//to our blackboard component
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Sense updated");
	for (AActor* Actor : DetectedActors)
	{
		if (Cast<AItem>(Actor) && !GetSeeingPawn())
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Actor Seen: %s"), *Actor->GetName()));
			BlackboardComp->SetValueAsObject(BlackboardTargetKey, Actor);
			return;
		}
	}

	//The character doesn't exist in our updated actors - so make sure
	//to delete any previous reference of him from the blackboard
	BlackboardComp->SetValueAsObject(BlackboardTargetKey, nullptr);
	*/
}
