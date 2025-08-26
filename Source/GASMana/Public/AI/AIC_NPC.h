// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIC_NPC.generated.h"

/**
 * 
 */
UCLASS()
class GASMANA_API AAIC_NPC : public AAIController
{
	GENERATED_BODY()
	AAIC_NPC();
	
	virtual void BeginPlay() override;

	UPROPERTY()
	bool bIsRanged;
public:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBlackboardData* BlackboardAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBlackboardComponent* BlackboardComp;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBehaviorTree* BehaviorTree;
};
