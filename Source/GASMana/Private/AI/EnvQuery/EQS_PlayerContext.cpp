// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnvQuery/EQS_PlayerContext.h"

#include "AI/AIC_NPC.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEQS_PlayerContext::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Provide Context Called");

	

	if (!GetWorld()) return;
	
	TArray<AActor*> OutActors;
	
	AAIC_NPC* AICon = Cast<AAIC_NPC>(Cast<AActor>(QueryInstance.Owner)->GetInstigatorController());

	//->AddOnScreenDebugMessage(-1, 1.f, FColor::Red, "Provide context called!");

	if (AICon)
	{
		if (const AActor* SeenPawn = AICon->GetSeeingPawn())
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Providing context: %s"), *SeenPawn->GetName()));
			UEnvQueryItemType_Actor::SetContextHelper(ContextData, SeenPawn);
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("No SeeingPawn!"));
		}
	}
}
