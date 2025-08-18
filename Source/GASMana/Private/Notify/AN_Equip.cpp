// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/AN_Equip.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagContainer.h"

void UAN_Equip::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	static const FGameplayTag EquipTag = FGameplayTag::RequestGameplayTag(FName("Event.Montage.Equip"));

	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EquipTag, FGameplayEventData());
}
