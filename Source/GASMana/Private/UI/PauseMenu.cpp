// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PauseMenu.h"

#include "ManaPlayerController.h"
#include "PlayerManaCharacter.h"
#include "Components/Button.h"
#include "Components/CheckBox.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"


void UPauseMenu::NativeConstruct()
{
	Super::NativeConstruct();

	QuitButton->OnClicked.AddUniqueDynamic(this, &UPauseMenu::QuitClicked);
	RestartButton->OnClicked.AddUniqueDynamic(this, &UPauseMenu::RestartClicked);
	ResumeButton->OnClicked.AddUniqueDynamic(this, &UPauseMenu::ResumeClicked);

	ScreenShakeCheckBox->OnCheckStateChanged.AddUniqueDynamic(this, &UPauseMenu::ScreenShakeCheckBoxStateChanged);

	CameraRotationSpeedSlider->OnValueChanged.AddUniqueDynamic(this, &UPauseMenu::CameraRotationSpeedChanged);
	CameraRotationSpeedSlider->SetValue(60.f);
	CameraRotationSpeedText->SetText(FText::FromString(FString::Printf(TEXT("Camera Rotation speed: %d"), 60)));


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

void UPauseMenu::ScreenShakeCheckBoxStateChanged(bool bIsChecked)
{
	if (const AManaPlayerController* PlayerController = Cast<AManaPlayerController>(GetOwningPlayer()))
	{
		if (APlayerManaCharacter* PChar = Cast<APlayerManaCharacter>(PlayerController->GetCharacter()))
		{
			PChar->bCanCameraShake = bIsChecked;
		}
	}
}

void UPauseMenu::CameraRotationSpeedChanged(float NewValue)
{
	if (const AManaPlayerController* PlayerController = Cast<AManaPlayerController>(GetOwningPlayer()))
	{
		if (APlayerManaCharacter* PChar = Cast<APlayerManaCharacter>(PlayerController->GetCharacter()))
		{
			int NewerValue = ceil(NewValue);
			PChar->CamRotSpeed = NewerValue;
			CameraRotationSpeedText->SetText(FText::FromString(FString::Printf(TEXT("Camera Rotation speed: %d"), NewerValue)));
		}
	}
}
