// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTTask_GoAroundTarget.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BTFunctionLibrary.h"


EBTNodeResult::Type UBTTask_GoAroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	UWorld* World = OwnerComp.GetWorld();
	if (!World) return EBTNodeResult::Failed;

	const UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(World);

	const AActor* TargetActor = UBTFunctionLibrary::GetBlackboardValueAsActor(this, TargetToFollow);
	
	FNavLocation Result;
	NavSys->GetRandomPointInNavigableRadius(TargetActor->GetActorLocation(), RadiusRange, Result);

	UBTFunctionLibrary::SetBlackboardValueAsVector(this, MoveToLocation, Result.Location);

	return EBTNodeResult::Succeeded;
}
