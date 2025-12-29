// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/AN_Ragdoll.h"

#include "GASMana/GASManaCharacter.h"

void UAN_Ragdoll::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                         const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	AGASManaCharacter* Character = Cast<AGASManaCharacter>(Owner);
	if (!Character) return;

	Character->Ragdoll();
}
