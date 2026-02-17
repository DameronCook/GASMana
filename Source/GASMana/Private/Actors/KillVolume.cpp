// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/KillVolume.h"

#include "GASMana/GASManaCharacter.h"


AKillVolume::AKillVolume()
{
}

void AKillVolume::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red, "Overlapped something!!!");
	if (AGASManaCharacter* ManaChar = Cast<AGASManaCharacter>(Other))
	{
		ManaChar->Die(Other->GetActorLocation());
	}
}