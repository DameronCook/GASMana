// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/AN_SwordHit.h"
#include "GASMana/GASManaCharacter.h"

void UAN_SwordHit::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (MeshComp)
	{
		if (AGASManaCharacter* AManaCharacter = Cast<AGASManaCharacter>(MeshComp->GetOwner()))
		{
			//TODO: Make this match with the sword attack socket instead
			AManaCharacter->MeleeAttackNotify(MeshComp->GetSocketLocation(FName("SwordVFXSocket")), bIsFinisher);
		}
	}
}
