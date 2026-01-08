// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ManaPlayerController.generated.h"

class UDeathMenu;
class UFadeOutScreen;
/**
 * 
 */
UCLASS()
class GASMANA_API AManaPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	void OnWindowFocusChanged(bool bIsFocused);

	UFUNCTION()
	void OnLevelLoaded(UWorld* LoadedWorld);

private:
	
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UFadeOutScreen> FadeWidgetBP;

	UPROPERTY()
	UFadeOutScreen* FadeWidget;
	
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UDeathMenu> DeathMenuWidgetBP;

	UPROPERTY()
	UDeathMenu* DeathMenuWidget;

public:
	FORCEINLINE UFadeOutScreen* GetFadeWidget() const { return FadeWidget; }
	FORCEINLINE UDeathMenu* GetDeathMenu() const { return DeathMenuWidget; }
};
