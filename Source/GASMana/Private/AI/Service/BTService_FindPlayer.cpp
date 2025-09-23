// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Service/BTService_FindPlayer.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

void UBTService_FindPlayer::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (!BlackboardComponent) return;
	
	TArray<AActor*> OutActors;
	
	//TODO: This probably needs to change to a visibility or eqs check at some point
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerClass, OutActors);
	

	if (OutActors[0])
	{
		BlackboardComponent->SetValueAsObject("TargetToFollow", OutActors[0]);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "I couldn't find any equipment!");
	}
}
