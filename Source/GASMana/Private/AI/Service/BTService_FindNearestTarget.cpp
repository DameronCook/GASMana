// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_FindNearestTarget.h"

#include "BehaviorTree/BTFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	float ClosestDistance = BIG_NUMBER;

	FName TagToSearch = "Enemy";
	if (AActor* OwnerActor = OwnerComp.GetOwner())
	{
		if (OwnerActor->ActorHasTag("Player"))
		{
			TagToSearch = TEXT("Player");
		}

		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), TagToSearch, OutActors);

		for (AActor* Actor : OutActors)
		{
			float TempDist = OwnerActor->GetDistanceTo(Actor);

			if (TempDist < ClosestDistance) ClosestDistance = TempDist;

			UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollow, Actor);
		}
		UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistToTarget, ClosestDistance);
	}
	
}
