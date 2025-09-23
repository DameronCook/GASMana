// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindBestWeapon.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API UBTService_FindBestWeapon : public UBTService
{
	GENERATED_BODY()

	UBTService_FindBestWeapon();
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector TargetToFollow;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Blackboard")
	FBlackboardKeySelector DistToTarget;

	UPROPERTY(EditAnywhere, Category = "Blackboard")
	TSubclassOf<AActor> ItemClass;
};
