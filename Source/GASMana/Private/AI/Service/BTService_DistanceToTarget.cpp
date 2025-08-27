// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_DistanceToTarget.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BTFunctionLibrary.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"


void UBTService_DistanceToTarget::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	Super::OnSearchStart(SearchData);

	OwnerActor = Cast<AActor>(SearchData.OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor"));
	GetDistanceBetweenActors(SearchData.OwnerComp);
}

void UBTService_DistanceToTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	GetDistanceBetweenActors(OwnerComp);
}

float UBTService_DistanceToTarget::GetDistance(const AActor* ActorOne, const AActor* ActorTwo)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, FString::Printf(TEXT("Distance: %.2f"), ActorOne->GetDistanceTo(ActorTwo)));
	return ActorOne->GetDistanceTo(ActorTwo);
}

void UBTService_DistanceToTarget::GetDistanceBetweenActors(UBehaviorTreeComponent& OwnerComp) const
{
	float Dist = BIG_NUMBER;
	if (const AActor* TargetActor = Cast<const AActor>(
		OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetToFollow"));
		OwnerActor && TargetActor)
	{
		Dist = GetDistance(OwnerActor, TargetActor);
	}

	if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsFloat("DistToTarget", Dist);
	}
}
