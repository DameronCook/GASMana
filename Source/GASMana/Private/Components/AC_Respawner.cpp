// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AC_Respawner.h"

#include "ManaPlayerController.h"
#include "PlayerManaCharacter.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UAC_Respawner::UAC_Respawner()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	RespawnPoints.Empty();
}

void UAC_Respawner::Respawn()
{
	float SmallestDist = BIG_NUMBER;
	const AActor* BestActor = nullptr;
	for (const AActor* CurrentPoint : RespawnPoints)
	{
		const float TempDist = CurrentPoint->GetHorizontalDistanceTo(GetOwner());
		if (TempDist <= SmallestDist)
		{
			BestActor = CurrentPoint;
			SmallestDist = TempDist;
		}
	}

	if (APlayerManaCharacter* Character = Cast<APlayerManaCharacter>(GetOwner()))
	{
		if (UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent())
		{
			ASC->ApplyGameplayEffectToSelf(Character->GetDamageEffectClass()->GetDefaultObject<UGameplayEffect>(), 1.f, ASC->MakeEffectContext());
		}
		if (AManaPlayerController* Controller = Cast<AManaPlayerController>(Character->GetController()))
		{
			Controller->RespawnLocation = BestActor->GetActorLocation() + FVector(0, 0, 100.f);
		}
	}
	//GetOwner()->SetActorLocation(BestActor->GetActorLocation());
}


// Called when the game starts
void UAC_Respawner::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), RespawnPoint, RespawnPoints);
}
