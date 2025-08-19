// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Item.h"

#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Interface/PlayerCharacterInterface.h"
// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMesh"));
	ItemMesh->SetupAttachment(RootComponent);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCol"));
	SphereComponent->SetupAttachment(RootComponent);

	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionObjectType(ECC_WorldDynamic);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}

void AItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetItem();
}

void AItem::SetItem()
{
	if (ItemData.ItemID.DataTable)
	{
		static const FString ContextString(TEXT("Finding row in Item Data Table"));
		if (const FPickUpItems* ItemRow = ItemData.ItemID.DataTable->FindRow<FPickUpItems>(ItemData.ItemID.RowName, ContextString))
		{
			ItemMesh->SetStaticMesh(ItemRow->ItemMesh);
		}
	}
}

void AItem::DisablePickUpCollision() const
{
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	SphereComponent->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

float AItem::TransformedSin() const
{
	return Amplitude * FMath::Sin(RunningTime * TimeConstant);
}

float AItem::TransformedCos() const
{
	return Amplitude * FMath::Cos(RunningTime * TimeConstant);

}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                            UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.F, FColor::Red, "OnSphere Overlap");
	if (IPlayerCharacterInterface* PlayerInterface = Cast<IPlayerCharacterInterface>(OtherActor))
	{
		PlayerInterface->SetOverlappingItem(this);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IPlayerCharacterInterface* PlayerInterface = Cast<IPlayerCharacterInterface>(OtherActor))
	{
		PlayerInterface->SetOverlappingItem(nullptr);
	}
}

// Called every frame
void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RunningTime += DeltaTime;

	if (ItemState == EItemState::EIS_Hovering)
	{
		AddActorWorldOffset(FVector(0.f, 0.f, TransformedSin()));
	}
}

void AItem::DisableSphereCollision() const
{
	if (SphereComponent)
	{
		SphereComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

