// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_FindBestWeapon.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Item/Equipment.h"
#include "Kismet/GameplayStatics.h"

UBTService_FindBestWeapon::UBTService_FindBestWeapon()
{
}

void UBTService_FindBestWeapon::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "I am running the service!");

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return;
	
	UObject* Target = BlackboardComponent->GetValueAsObject("TargetToFollow");
	const AItem* Item = Cast<AItem>(Target);
	
	if (Item && !BlackboardComponent->GetValueAsBool("IsMyItemPickedUp"))
	{
		if (Item->IsPickedUp())
		{
			BlackboardComponent->SetValueAsBool("IsMyItemPickedUp", true);
		}
	}
	
	/* If we don't have a current target we find one */
	if (const AActor* OwnerActor = OwnerComp.GetOwner())
	{
		TArray<AActor*> OutActors;
		
		//TODO: This probably needs to change to a visibility or eqs check at some point
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ItemClass, OutActors);

		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Number of Players found: %d"), OutActors.Num()));
		AActor* NearestTarget = nullptr;
		float ClosestDistance = BIG_NUMBER;
		for (AActor* Actor : OutActors)
		{
			if (const AEquipment* Equipment = Cast<AEquipment>(Actor)) 
			{
				if (!Equipment->IsPickedUp())
				{
					if (const float TempDist = OwnerActor->GetDistanceTo(Actor); TempDist < ClosestDistance)
					{
						ClosestDistance = TempDist;
						NearestTarget = Actor;
					}
				}
			}
		}

		if (NearestTarget)
		{
			BlackboardComponent->SetValueAsObject("TargetToFollow", NearestTarget);
			BlackboardComponent->SetValueAsBool("IsMyItemPickedUp", false);
		}
	}
}
