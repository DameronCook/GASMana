// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/AN_SwordHit.h"
#include "GASMana/GASManaCharacter.h"

void UAN_SwordHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	AGASManaCharacter* AManaCharacter = Cast<AGASManaCharacter>(MeshComp->GetOwner());

	if (AManaCharacter)
	{
		AManaCharacter->HandleMelee();
	}
	//Character
}
