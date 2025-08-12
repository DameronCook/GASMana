// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/AN_SetFree.h"
#include "GameplayTagContainer.h"
#include "../GASManaCharacter.h"


void UAN_SetFree::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (AGASManaCharacter* Character = Cast<AGASManaCharacter>(Owner))
	{
		static const FGameplayTag FreeTag = FGameplayTag::RequestGameplayTag(FName("Character.IsFree"));

		Character->GetAbilitySystemComponent()->AddLooseGameplayTag(FreeTag);
	}
}