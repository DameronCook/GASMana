// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/ManaHookParent.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Components/BillboardComponent.h"
#include "Components/WidgetComponent.h"



// Sets default values
AManaHookParent::AManaHookParent()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh")); 

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCol"));
	Sphere->SetSphereRadius(32.f);
	Sphere->SetupAttachment(Mesh);

	switch (GrappleType)
	{
	case EGrappleType::E_ZipToPoint:
		Mesh->SetStaticMesh(ZipToPointMesh);
		WidgetClass = ZipToPointWidget;
		break;
	case EGrappleType::E_Launch:
		Mesh->SetStaticMesh(LaunchMesh);
		WidgetClass = LaunchWidget;
		break;
	case EGrappleType::E_LaunchUp:
		Mesh->SetStaticMesh(LaunchUpMesh);
		WidgetClass = LunchUpWidget;
		break;
	case EGrappleType::E_Swing:
		Mesh->SetStaticMesh(SwingMesh);
		WidgetClass = SwingWidget;
		break;
	default:
		break;
	}
}

void AManaHookParent::BeginPlay()
{
	Super::BeginPlay();
}

FVector AManaHookParent::GetCharacterTargetLocation()
{
	return GetActorTransform().GetLocation() - CharacterOffset;
}


