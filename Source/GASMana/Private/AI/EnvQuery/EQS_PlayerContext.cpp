// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnvQuery/EQS_PlayerContext.h"

#include "AI/AIC_NPC.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEQS_PlayerContext::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	if (!GetWorld()) return;
	
	TArray<AActor*> OutActors;
	
	AAIC_NPC* AICon = Cast<AAIC_NPC>(Cast<AActor>(QueryInstance.Owner)->GetInstigatorController());

	//->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Provide context called!");

	/* we are getting a controller, but we aren't getting the seeing pawn */
	if (AICon && AICon->GetSeeingPawn())
	{
		UEnvQueryItemType_Actor::SetContextHelper(ContextData, AICon->GetSeeingPawn());
	}
}
