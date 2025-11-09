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
	Sight->PeripheralVisionAngleDegrees = 90.f;
 
	//Tell the sight sense to detect everything
	Sight->DetectionByAffiliation.bDetectEnemies = true;
	Sight->DetectionByAffiliation.bDetectFriendlies = false;
	Sight->DetectionByAffiliation.bDetectNeutrals = true;
 
	//Register the sight sense to our Perception Component
	AIPerceptionComponent->ConfigureSense(*Sight);

}

void AAIC_NPC::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Actor->ActorHasTag("Player"))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			//TODO: Stop using the sensed actor variable for attacks. Rely on tags.
			SensedActor = Actor;

			GetWorldTimerManager().ClearTimer(PlayerPerceptionTimer);
			PlayerPerceptionTimer.Invalidate();

			//TODO: Make the can see player only true if the enemy has full sight of the player
			BlackboardComp->SetValueAsBool("CanSeePlayer", true);
			BlackboardComp->SetValueAsObject("TargetToFollow", Actor);
		}
		else
		{
			GetWorldTimerManager().SetTimer(PlayerPerceptionTimer, this, &AAIC_NPC::ClearTargetToFollow, TimeToLosePlayer, false);
			BlackboardComp->SetValueAsBool("CanSeePlayer", false);
		}
	}
	else if (AEquipment* Equipment = Cast<AEquipment>(Actor))
	{
		SensedEquipment.AddUnique(Equipment);
	}
}

void AAIC_NPC::ClearTargetToFollow() const
{
	BlackboardComp->SetValueAsObject("TargetToFollow", nullptr);
	BlackboardComp->SetValueAsBool("NoticedPlayerBefore", false);
}

void AAIC_NPC::BeginPlay()
{
	Super::BeginPlay();
	
	if (BlackboardAsset) UseBlackboard(BlackboardAsset, BlackboardComp);

	if (BlackboardComp) BlackboardComp->SetValueAsBool("IsRanged", bIsRanged);

	RunBehaviorTree(BehaviorTree);

	AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AAIC_NPC::OnTargetPerceptionUpdated);
}

void AAIC_NPC::Possess(APawn* InPawn)
{
	Super::Possess(InPawn);
	
	if (BlackboardAsset) UseBlackboard(BlackboardAsset, BlackboardComp);

	if (BlackboardComp) BlackboardComp->SetValueAsBool("IsRanged", bIsRanged);

	ActivateTree();
}

void AAIC_NPC::ActivateTree()
{
	RunBehaviorTree(BehaviorTree);

	if (BlackboardComp) BlackboardComp->SetValueAsBool("AmILoaded", true);
}

AActor* AAIC_NPC::GetSeeingPawn() const
{
	return SensedActor ?  Cast<AActor>(SensedActor) : nullptr;
}
