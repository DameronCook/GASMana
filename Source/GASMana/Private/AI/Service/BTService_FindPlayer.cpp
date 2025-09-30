// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_FindPlayer.h"

#include "AIController.h"
#include "AI/AIC_NPC.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Tasks/BTTask_RunEQSQuery.h"

UBTService_FindPlayer::UBTService_FindPlayer()
{
	NodeName = "Find Player";
	Interval = 0.5f;
	bNotifyTick = true; 
}


void UBTService_FindPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return;

	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return;
	
	if (const AAIC_NPC* Controller = Cast<AAIC_NPC>(AIController))
	{
		if (AActor* Actor = Controller->GetSensedActor())
		{
			BlackboardComponent->SetValueAsObject("TargetToFollow", Actor);
		}
	}
}
