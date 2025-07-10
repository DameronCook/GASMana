// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SphereComponent.h"
#include "Materials/MaterialInterface.h"
#include "ManaHookParent.generated.h"

UENUM(BlueprintType)
enum class EGrappleType : uint8
{
	E_ZipToPoint  UMETA(DisplayName = "ZipToPoint"),
	E_Launch UMETA(DisplayName = "Launch"),
	E_LaunchUp UMETA(DisplayName = "LaunchUp"),
	E_Swing UMETA(DisplayName = "Swing")
};

class UWidgetComponent;

UCLASS()
class GASMANA_API AManaHookParent : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USphereComponent* Sphere;
	/** HUD Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> WidgetClass;
	/** HUD Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> ZipToPointWidget;
	/** HUD Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> LaunchWidget;
	/** HUD Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> LunchUpWidget;
	/** HUD Class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = UI, meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> SwingWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	EGrappleType GrappleType;



	///MESHES WHOOOOOOOOOOOOOOOO
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* ZipToPointMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* LaunchMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* LaunchUpMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	UStaticMesh* SwingMesh;


	UPROPERTY()
	FVector CharacterOffset = FVector(0.f, 0.f, 25.f);
public:	
	AManaHookParent();

	UFUNCTION(BlueprintImplementableEvent)
	void SetActive(bool bActive);

	FVector GetCharacterTargetLocation();

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UWidgetComponent> TargetTypeWidget;

	FORCEINLINE EGrappleType GetGrappleType() { return GrappleType; }


protected:
	virtual void BeginPlay() override;

};
