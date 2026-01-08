// Fill out your copyright notice in the Description page of Project Settings.


#include "ManaPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "UI/DeathMenu.h"
#include "UI/FadeOutScreen.h"

void AManaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FadeWidget = CreateWidget<UFadeOutScreen>(this, FadeWidgetBP);
	FadeWidget->AddToViewport();
	
	// Bind the delegates to the animation events if the animation is valid
	if (FadeWidget->GetFadeInAnim())
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Fade Delegates bound?");
		FadeWidget->BindToAnimationStarted(FadeWidget->GetFadeInAnim(), FadeWidget->GetFadeInAnimStartDelegate());
		FadeWidget->BindToAnimationFinished(FadeWidget->GetFadeInAnim(), FadeWidget->GetFadeInAnimEndDelegate());
		FadeWidget->BindToAnimationStarted(FadeWidget->GetFadeOutAnim(), FadeWidget->GetFadeOutAnimStartDelegate());
		FadeWidget->BindToAnimationFinished(FadeWidget->GetFadeOutAnim(), FadeWidget->GetFadeOutAnimEndDelegate());
	}

	DeathMenuWidget = CreateWidget<UDeathMenu>(this, DeathMenuWidgetBP);
	
	FSlateApplication::Get().OnApplicationActivationStateChanged().AddUObject(this, &AManaPlayerController::OnWindowFocusChanged);

	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &AManaPlayerController::OnLevelLoaded);
}

void AManaPlayerController::OnWindowFocusChanged(bool bIsFocused)
{
	// Don't pause in the editor, it's annoying
#if !WITH_EDITOR
	if (bIsFocused)
	{
		// Unlimit game FPS
		GEngine->SetMaxFPS(0);

		// Unpause the game
		// MyHUD->SetPause(false);
	}
	else
	{
		// Reduce FPS to max 10 while in the background
		GEngine->SetMaxFPS(10.0f);

		// Pause the game, using your "show pause menu" function
		// MyHUD->SetPause(true);
	}
#endif
}

void AManaPlayerController::OnLevelLoaded(UWorld* LoadedWorld)
{
	if (FadeWidget)
	{
		FadeWidget->FadeToNothing();
	}

	SetInputMode(FInputModeGameOnly());
}
