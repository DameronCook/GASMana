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

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCol"));
	Sphere->SetSphereRadius(32.f);
	Sphere->SetupAttachment(Mesh);

	switch (GrappleType)
	{
	case EGrappleType::E_ZipToPoint:
		WidgetClass = ZipToPointWidget;
		break;
	case EGrappleType::E_Launch:
		WidgetClass = LaunchWidget;
		break;
	case EGrappleType::E_LaunchUp:
		WidgetClass = LunchUpWidget;
		break;
	case EGrappleType::E_Swing:
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


