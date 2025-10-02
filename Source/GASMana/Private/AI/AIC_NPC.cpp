// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AIC_NPC.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Item/Equipment.h"
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

	AIPerceptionComponent->OnPerceptionUpdated.AddDynamic(this, &AAIC_NPC::OnSenseUpdated);
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
	return SensedActor ?  Cast<AActor>(SensedActor) : nullptr;
}

void AAIC_NPC::OnSenseUpdated(const TArray<AActor*>& DetectedActors)
{
	for (AActor* Actor : DetectedActors)
	{
		if (AEquipment* Equipment = Cast<AEquipment>(Actor))
		{
			SensedEquipment.AddUnique(Equipment);
		}
		if (Cast<APawn>(Actor))
		{
			SensedActor = Actor;
			return;
		}
	}
	//SensedActor = nullptr;
}
