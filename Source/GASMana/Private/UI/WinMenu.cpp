// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WinMenu.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UWinMenu::NativeConstruct()
{
	Super::NativeConstruct();

	QuitButton->OnClicked.AddUniqueDynamic(this, &UWinMenu::QuitClicked);
	RestartButton->OnClicked.AddUniqueDynamic(this, &UWinMenu::RestartClicked);
}

void UWinMenu::RestartClicked()
{
	FName CurrentLevelName = FName(*UGameplayStatics::GetCurrentLevelName(GetWorld()));
	UGameplayStatics::OpenLevel(GetWorld(), CurrentLevelName);
}

void UWinMenu::QuitClicked()
{
	APlayerController* SpecificPlayer = GetWorld()->GetFirstPlayerController();
	UKismetSystemLibrary::QuitGame(GetWorld(), SpecificPlayer, EQuitPreference::Quit, false);
}
