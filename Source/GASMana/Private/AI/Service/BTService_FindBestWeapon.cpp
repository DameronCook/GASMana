// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_FindBestWeapon.h"

#include "AI/AIC_NPC.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Item/Equipment.h"

UBTService_FindBestWeapon::UBTService_FindBestWeapon()
{
}

void UBTService_FindBestWeapon::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return;

	AAIController* OwnerController = OwnerComp.GetAIOwner();
	if (!OwnerController) return;

	AAIC_NPC* NPCController = Cast<AAIC_NPC>(OwnerController);
	if (!NPCController) return;

	
	UObject* Target = BlackboardComponent->GetValueAsObject("TargetToFollow");
	const AItem* Item = Cast<AItem>(Target);
	
	if (Item && !BlackboardComponent->GetValueAsBool("IsMyItemPickedUp"))
	{
		if (Item->IsPickedUp())
		{
			BlackboardComponent->SetValueAsBool("IsMyItemPickedUp", true);
			//NPCController->GetSensedEquipment().Remove();
		}
	}

	/* If we don't have a current target we find one */
	if (const AActor* OwnerActor = Cast<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("SelfActor")))
	{
		AActor* NearestTarget = nullptr;
		float ClosestDistance = BIG_NUMBER;
		for (AEquipment* Equipment : NPCController->GetSensedEquipment())
		{
			if (Equipment != nullptr)
			{
				if (Equipment->GetClass() == ItemClass)
				{
					if (!Equipment->IsPickedUp())
					{
						if (const float TempDist = OwnerActor->GetDistanceTo(Equipment); TempDist < ClosestDistance)
						{
							GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, "I HAVE FOUND A WEAPON");
							ClosestDistance = TempDist;
							NearestTarget = Equipment;
						}
					}
				}
			}
			else
			{
				NPCController->GetSensedEquipment().Remove(Equipment);
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Called Remove on Sensed equipment");
			}
		}

		if (NearestTarget)
		{
			GEngine->AddOnScreenDebugMessage(-1, 20.f, FColor::Red, "I HAVE FOUND A WEAPON AND SET MY TARGET TO FOLLOW");
			BlackboardComponent->SetValueAsObject("TargetToFollow", NearestTarget);
			BlackboardComponent->SetValueAsFloat("DistToTarget", ClosestDistance);
			BlackboardComponent->SetValueAsBool("IsMyItemPickedUp", false);
		}
	}
}
