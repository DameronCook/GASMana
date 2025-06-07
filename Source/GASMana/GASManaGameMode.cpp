// Copyright Epic Games, Inc. All Rights Reserved.

#include "GASManaGameMode.h"
#include "PlayerManaCharacter.h"
#include "UObject/ConstructorHelpers.h"

AGASManaGameMode::AGASManaGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/GASMana/Blueprints/Actors/BP_PlayerManaCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
