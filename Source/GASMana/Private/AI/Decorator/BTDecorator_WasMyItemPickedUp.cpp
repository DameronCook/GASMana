// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_WasMyItemPickedUp.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Item/Equipment.h"

UBTDecorator_WasMyItemPickedUp::UBTDecorator_WasMyItemPickedUp()
{
	FlowAbortMode = EBTFlowAbortMode::Self;
}

bool UBTDecorator_WasMyItemPickedUp::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
<<<<<<< Updated upstream
	if (const AEquipment* Equipment = Cast<AEquipment>(OwnerComp.GetBlackboardComponent()->GetValueAsObject("TargetToFollow")))
	{
		return !Equipment->IsPickedUp();
	}
	return false;
=======
	const UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComp) return false;
	
	const AItem* Item = Cast<AItem>(BlackboardComp->GetValueAsObject("TargetToFollow"));
	if (!Item) return false;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple,
		FString::Printf(TEXT("My Item was picked up: %s"), Item->IsPickedUp() ? TEXT("true") : TEXT("false")));
	return Item->IsPickedUp();
>>>>>>> Stashed changes
}
