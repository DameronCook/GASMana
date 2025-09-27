// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "AIC_NPC.generated.h"

class UAISenseConfig_Sight;
class UBehaviorTreeComponent;
/**
 * 
 */
UCLASS()
class GASMANA_API AAIC_NPC : public AAIController
{
	GENERATED_BODY()
	AAIC_NPC();
	
	/** BlackboardComponent - used to initialize blackboard values and set/get values from a blackboard asset */
	UPROPERTY()
	UBlackboardComponent* BlackboardComp;
 
	/** BehaviorTreeComponent - used to start a behavior tree */
	UPROPERTY()
	UBehaviorTreeComponent* BehaviorTreeComp;

	UPROPERTY()
	bool bIsRanged;

	/** The function that fires when the perception of our AI gets updated */
	UFUNCTION()
	void OnSenseUpdated(const TArray<AActor*>& DetectedActors);
 
	/** A Sight Sense config for our AI */
	UPROPERTY()
	UAISenseConfig_Sight* Sight;

	FName BlackboardTargetKey = FName("TargetToFollow");

	AActor* SensedActor;

protected:
	/** The Behavior Tree that contains the logic of our AI */
	UPROPERTY(EditAnywhere)
	UBehaviorTree* BehaviorTree;

	/** The Perception Component of our AI */
	UPROPERTY(VisibleAnywhere)
	UAIPerceptionComponent* AIPerceptionComponent;
	
public:
	virtual void BeginPlay() override;

	virtual void Possess(APawn* InPawn) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	UBlackboardData* BlackboardAsset;

	/** Returns the seeing pawn. Returns null, if our AI has no target */
	AActor* GetSeeingPawn() const;
};
