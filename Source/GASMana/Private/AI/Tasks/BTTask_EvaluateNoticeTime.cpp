// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/BTTask_EvaluateNoticeTime.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_EvaluateNoticeTime::UBTTask_EvaluateNoticeTime()
{
	
}

EBTNodeResult::Type UBTTask_EvaluateNoticeTime::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Blackboard) return EBTNodeResult::Failed;

	const float Dist = Blackboard->GetValueAsFloat("DistToTarget");

	float TimeToWait = UKismetMathLibrary::MapRangeClamped(Dist, MinDist, MaxDistance, MinTimeToWait, MaxTimeToWait);

	Blackboard->SetValueAsFloat("InvestigateTime", TimeToWait);

	return EBTNodeResult::Succeeded;
	
}
