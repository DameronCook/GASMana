// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "Item.generated.h"

class UTexture2D;
class USphereComponent;

UENUM(BlueprintType)
enum class EItemType : uint8
{
	EIT_Sword UMETA(DisplayName = "Sword"),
	EIT_Shield UMETA(DisplayName = "Shield"),
	EIT_Money UMETA(DisplayName = "Money"),
	EIT_Consumable UMETA(DisplayName = "Consumable")
};

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

USTRUCT(BlueprintType)
struct FPlayerItemSlot
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FDataTableRowHandle ItemID;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int Amount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EItemType ItemType;
};

USTRUCT(BlueprintType)
struct FPickUpItems : public FTableRowBase
{
	GENERATED_BODY()
	
	/** Text **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText ItemName;    /** Name of the Item **/
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite) 
	FText ItemDescription; /** Description of the Item **/

	/** Widget Stuff **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemIcon; /** Widget Texture of the Item to display on menus **/

	/** Mesh **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMesh* ItemMesh;
	
};

UCLASS()
class GASMANA_API AItem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	virtual void OnConstruction(const FTransform& Transform) override;

	virtual void Tick(float DeltaTime) override;

	void DisableSphereCollision() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float Amplitude = 0.25f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine Parameters")
	float TimeConstant = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float RotationRate = 90.f;
	
	UFUNCTION(BlueprintCallable)
	void SetItem() const;

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure)
	float TransformedSin() const;

	UFUNCTION(BlueprintPure)
	float TransformedCos() const;

	template<typename T>
	T Avg(T First, T Second);
	
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPlayerItemSlot ItemData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Hooks, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* ItemMesh;
	
	UPROPERTY(VisibleAnywhere)
	USphereComponent* SphereComponent;

	EItemState ItemState = EItemState::EIS_Hovering;
	
private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float RunningTime;
	
};

template<typename T>
inline T AItem::Avg(T first, T second)
{
	return T(first + second) / 2;
}