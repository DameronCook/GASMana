// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/KillVolume.h"

#include "ManaPlayerController.h"
#include "PlayerManaCharacter.h"
#include "Components/AC_Respawner.h"
#include "GASMana/GASManaCharacter.h"
#include "UI/FadeOutScreen.h"


AKillVolume::AKillVolume()
{
}

void AKillVolume::NotifyActorBeginOverlap(AActor* Other)
{
	Super::NotifyActorBeginOverlap(Other);
	GEngine->AddOnScreenDebugMessage(-1,5.f,FColor::Red, "Overlapped something!!!");
	if (AGASManaCharacter* ManaChar = Cast<AGASManaCharacter>(Other))
	{
		if (APlayerManaCharacter* PlayerChar = Cast<APlayerManaCharacter>(ManaChar))
		{
			if (UAC_Respawner* Respawner = PlayerChar->GetRespawnComponent())
			{
				AManaPlayerController* Controller = Cast<AManaPlayerController>(PlayerChar->GetController());
				if (Controller)
				{
					Respawner->Respawn();
					if (PlayerChar->IsAlive())
					{
						Controller->bDidIFall = true;
					}
					if (Controller->GetFadeWidget())
					{
						Controller->GetFadeWidget()->StartFadeToBlack();
					}
				}
			}
			else
			{
				ManaChar->Die(Other->GetActorLocation());
			}
		}
		else
		{
			ManaChar->Die(Other->GetActorLocation());
		}
	}
}