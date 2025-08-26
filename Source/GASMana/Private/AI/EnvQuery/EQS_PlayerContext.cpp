// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/EnvQuery/EQS_PlayerContext.h"

#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"
#include "Kismet/GameplayStatics.h"

void UEQS_PlayerContext::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	if (!GetWorld()) return;
	
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), PlayerClass, OutActors);
	
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, OutActors);
}
