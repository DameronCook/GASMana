// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GoAroundTarget.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"


EBTNodeResult::Type UBTTask_GoAroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	UWorld* World = OwnerComp.GetWorld();
	if (!World) return EBTNodeResult::Failed;

	const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent)
	{
		if (const AActor* TargetActor = Cast<const AActor>(BlackboardComponent->GetValueAsObject("TargetToFollow")))
		{
			FNavLocation Result;
			NavSys->GetRandomPointInNavigableRadius(TargetActor->GetActorLocation(), RadiusRange, Result);

			BlackboardComponent->SetValueAsVector("MoveToLocation",Result.Location);
		}
	}
	return EBTNodeResult::Succeeded;
		
}
