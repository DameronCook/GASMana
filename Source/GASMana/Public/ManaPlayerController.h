// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ManaPlayerController.generated.h"

class UEnemyCounter;
class UWinMenu;
class UPauseMenu;
class UDeathMenu;
class UFadeOutScreen;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameWonSignature);

UCLASS()
class GASMANA_API AManaPlayerController : public APlayerController
{
	GENERATED_BODY()

	AManaPlayerController();

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
	UDeathMenu* DeathMenu;

	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UPauseMenu> PauseMenuWidgetBP;

	UPROPERTY()
	UPauseMenu* PauseMenu;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UWinMenu> WinMenuBP;

	UPROPERTY()
	UWinMenu* WinMenu;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UEnemyCounter> EnemyCounterBP;

	UPROPERTY()
	UEnemyCounter* EnemyCounter;

public:
	UPROPERTY()
	bool bDidIWin;
	
	UPROPERTY()
	bool bDidIFall;

	UPROPERTY()
	FVector RespawnLocation;
	
	UFUNCTION()
	void ShowPauseMenu();

	UFUNCTION()
	void HidePauseMenu();

	FORCEINLINE UFadeOutScreen* GetFadeWidget() const { return FadeWidget; }
	FORCEINLINE UDeathMenu* GetDeathMenu() const { return DeathMenu; }
	FORCEINLINE UPauseMenu* GetPauseMenu() const { return PauseMenu; }
	FORCEINLINE UWinMenu* GetWinMenu() const { return WinMenu; }
	FORCEINLINE UEnemyCounter* GetEnemyCounter() const { return EnemyCounter; }
	
};
