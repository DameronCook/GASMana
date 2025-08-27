// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_FindNearestTarget.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);


	FName TagToSearch = "Player";
	if (const AActor* OwnerActor = OwnerComp.GetOwner())
	{
		AActor* NearestTarget = nullptr;
		float ClosestDistance = BIG_NUMBER;

		
		if (OwnerActor->ActorHasTag("Player"))
		{
			TagToSearch = TEXT("Enemy");
		}

		TArray<AActor*> OutActors;
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), TagToSearch, OutActors);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Number of Players found: %d"), OutActors.Num()));
		for (AActor* Actor : OutActors)
		{
			if (float TempDist = OwnerActor->GetDistanceTo(Actor); TempDist < ClosestDistance)
			{
				ClosestDistance = TempDist;
				NearestTarget = Actor;
			}
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Nearest Player's Name: %s"), *NearestTarget->GetName()));

		if (UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent(); BlackboardComponent &&
			NearestTarget)
		{
			BlackboardComponent->SetValueAsObject("TargetToFollow", NearestTarget);
			BlackboardComponent->SetValueAsFloat("DistToTarget", ClosestDistance);
		}
		//UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollow, NearestTarget);
		//UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistToTarget, ClosestDistance);
	}
}
