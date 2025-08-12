// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/AN_ComboNotifyState.h"
#include "../GASManaCharacter.h"

void UAN_ComboNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	AGASManaCharacter* Character = Cast<AGASManaCharacter>(MeshComp->GetOwner());

	if (Character)
	{
		Character->SetNextComboSegment(NextComboAttack);
	}
}

void UAN_ComboNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AGASManaCharacter* Character = Cast<AGASManaCharacter>(MeshComp->GetOwner());

	if (Character)
	{
		Character->SetDefaultCombos();
		GEngine->AddOnScreenDebugMessage(2, 5.f, FColor::Red, "Notify End read.");

	}
}
