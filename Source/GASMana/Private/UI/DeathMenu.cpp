// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/DeathMenu.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UDeathMenu::NativeConstruct()
{
	Super::NativeConstruct();

	QuitButton->OnClicked.AddUniqueDynamic(this, &UDeathMenu::QuitClicked);
	RestartButton->OnClicked.AddUniqueDynamic(this, &UDeathMenu::RestartClicked);
}

void UDeathMenu::RestartClicked()
{
	/* Restart Game? */
	FName CurrentLevelName = FName(*UGameplayStatics::GetCurrentLevelName(GetWorld()));

	UGameplayStatics::OpenLevel(GetWorld(), CurrentLevelName);
	
}

void UDeathMenu::QuitClicked()
{
	APlayerController* SpecificPlayer = GetWorld()->GetFirstPlayerController();
	UKismetSystemLibrary::QuitGame(GetWorld(), SpecificPlayer, EQuitPreference::Quit, false);
}
