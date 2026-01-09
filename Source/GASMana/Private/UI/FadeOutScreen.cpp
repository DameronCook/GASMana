// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FadeOutScreen.h"

#include "ManaPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UI/DeathMenu.h"


void UFadeOutScreen::NativeConstruct()
{
	Super::NativeConstruct();
	
	// Bind the delegates to the C++ functions
	FadeOutStartDelegate.BindDynamic(this, &UFadeOutScreen::FadeOutStarted);
	FadeOutEndDelegate.BindDynamic(this, &UFadeOutScreen::FadeOutEnded);

	FadeInStartDelegate.BindDynamic(this, &UFadeOutScreen::FadeInStarted);
	FadeInEndDelegate.BindDynamic(this, &UFadeOutScreen::FadeInEnded);
}

void UFadeOutScreen::StartFadeToBlack()
{
	if (FadeOutAnim)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Play Animation called!");

		PlayAnimation(FadeOutAnim);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Fade out anim not found!");
	}
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UFadeOutScreen::FadeOutStarted()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Fade Started Delegate Called");
	GetWorld()->GetWorldSettings()->SetTimeDilation(0.333f);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void UFadeOutScreen::FadeOutEnded()
{
	GetWorld()->GetWorldSettings()->SetTimeDilation(1.f);
	UGameplayStatics::SetGamePaused(GetWorld(), true);
	if (AManaPlayerController* PlayerController = Cast<AManaPlayerController>(GetOwningPlayer()))
	{
		if (UDeathMenu* Menu = PlayerController->GetDeathMenu())
		{
			Menu->AddToViewport();

			Menu->PlayAnimation(Menu->GetStartAnimation());

			FInputModeUIOnly InputModeData;
			InputModeData.SetWidgetToFocus(Menu->TakeWidget()); 
			InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::LockAlways); 
			
			PlayerController->SetInputMode(InputModeData);

			Menu->SetUserFocus(PlayerController);
			
			PlayerController->bShowMouseCursor = true;
		}
	}
}

void UFadeOutScreen::FadeInStarted()
{
	
}

void UFadeOutScreen::FadeInEnded()
{
	
}


void UFadeOutScreen::FadeToNothing()
{
	if (FadeInAnim)
	{
		PlayAnimation(FadeInAnim);
	}
}
