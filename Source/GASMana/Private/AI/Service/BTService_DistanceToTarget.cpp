// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_DistanceToTarget.h"
#include "BehaviorTree/BTFunctionLibrary.h"



void UBTService_DistanceToTarget::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	Super::OnSearchStart(SearchData);

	OwnerActor = SearchData.OwnerComp.GetOwner();
	GetDistanceBetweenActors();
}

void UBTService_DistanceToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	GetDistanceBetweenActors();
}

float UBTService_DistanceToTarget::GetDistance(const AActor* ActorOne, const AActor* ActorTwo)
{
	return ActorOne->GetDistanceTo(ActorTwo);
}

void UBTService_DistanceToTarget::GetDistanceBetweenActors()
{
	AActor* TargetActor = UBTFunctionLibrary::GetBlackboardValueAsActor(this, TargetToFollow);

	if (OwnerActor && TargetActor)
	{
		GetDistance(OwnerActor, TargetActor);
	}
}