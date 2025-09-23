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
	
	if (const AActor* OwnerActor = OwnerComp.GetOwner())
	{
		TArray<AActor*> OutActors;
		
		//TODO: This probably needs to change to a visibility or eqs check at some point
		UGameplayStatics::GetAllActorsOfClassWithTag(GetWorld(), ItemClass,"Equipment", OutActors);

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
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "I couldn't find any equipment!");
		}
	}
}
