// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/AN_DropItem.h"
#include "PlayerManaCharacter.h"

void UAN_DropItem::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                         const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	APlayerManaCharacter* Character = Cast<APlayerManaCharacter>(Owner);
	if (!Character) return;

	if (bDropRight)
	{
		Character->DropRightEquipment(Character->GetRightHandEquipment());
	}
	else
	{
		Character->DropLeftEquipment(Character->GetLeftHandEquipment());

	}
}
