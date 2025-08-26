// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DistanceToTarget.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UBTService_DistanceToTarget : public UBTService
{
	GENERATED_BODY()

	void GetDistanceBetweenActors(UBehaviorTreeComponent& OwnerComp) const;
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	static float GetDistance(const AActor* ActorOne, const AActor* ActorTwo); 

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetToFollow;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DistToTarget;

	UPROPERTY()
	AActor* OwnerActor;
};
