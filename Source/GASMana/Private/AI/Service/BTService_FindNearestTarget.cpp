// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_FindNearestTarget.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindNearestTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return;

	
	FName TagToSearch = "Player";
	if (const AActor* OwnerActor = OwnerComp.GetOwner())
	{
		//See if we have anything in our left hand first
		if (!BlackboardComponent->GetValueAsObject("LeftHandEquipment"))
		{
			TagToSearch = "LeftHandEquipment";
		}

		//Then check right. This goes last as we should ALWAYS prioritize right hand equipment
		if (!BlackboardComponent->GetValueAsObject("RightHandEquipment"))
		{
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Not Getting Value");
			TagToSearch = "RightHandEquipment";
		}
		TArray<AActor*> OutActors;
		
		//TODO: This probably needs to change to a visibility or eqs check at some point
		UGameplayStatics::GetAllActorsWithTag(GetWorld(), TagToSearch, OutActors);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Number of Players found: %d"), OutActors.Num()));
		AActor* NearestTarget = nullptr;
		float ClosestDistance = BIG_NUMBER;
		for (AActor* Actor : OutActors)
		{
			if (const float TempDist = OwnerActor->GetDistanceTo(Actor); TempDist < ClosestDistance)
			{
				ClosestDistance = TempDist;
				NearestTarget = Actor;
			}
		}
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Nearest Player's Name: %s"), *NearestTarget->GetName()));

		if (NearestTarget)
		{
			BlackboardComponent->SetValueAsObject("TargetToFollow", NearestTarget);
			BlackboardComponent->SetValueAsFloat("DistToTarget", ClosestDistance);
		}
	}
}
