// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PauseMenu.h"

#include "ManaPlayerController.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


void UPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();

	QuitButton->OnClicked.AddUniqueDynamic(this, &UPauseMenu::QuitClicked);
	RestartButton->OnClicked.AddUniqueDynamic(this, &UPauseMenu::RestartClicked);
	ResumeButton->OnClicked.AddUniqueDynamic(this, &UPauseMenu::ResumeClicked);

	PauseAnimEndedDelegate.BindDynamic(this, &UPauseMenu::PauseAnimEnded);
}

void UPauseMenu::RestartClicked()
{
	/* Restart Game? */
	FName CurrentLevelName = FName(*UGameplayStatics::GetCurrentLevelName(GetWorld()));

	UGameplayStatics::OpenLevel(GetWorld(), CurrentLevelName);
}

void UPauseMenu::QuitClicked()
{
	APlayerController* SpecificPlayer = GetWorld()->GetFirstPlayerController();
	UKismetSystemLibrary::QuitGame(GetWorld(), SpecificPlayer, EQuitPreference::Quit, false);
}

void UPauseMenu::ResumeClicked()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	if (AManaPlayerController* PlayerController = Cast<AManaPlayerController>(GetOwningPlayer()))
	{
		PlayerController->HidePauseMenu();
	}
}

void UPauseMenu::PauseAnimEnded()
{
	//Nothing for now. Isn't bound correctly.
}