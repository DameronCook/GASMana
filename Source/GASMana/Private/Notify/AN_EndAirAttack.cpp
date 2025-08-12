// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/AN_EndAirAttack.h"
#include "GameplayTagContainer.h"
#include "../GASManaCharacter.h"

void UAN_EndAirAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (AGASManaCharacter* Character = Cast<AGASManaCharacter>(Owner))
	{
		FGameplayTagContainer AirAttackTags;
			
		static const FGameplayTag AttackTag = FGameplayTag::RequestGameplayTag(FName("Player.IsAirAttacking"));

		AirAttackTags.AddTag(AttackTag);

		if (Character->GetAbilitySystemComponent()->HasMatchingGameplayTag(AttackTag))
		{
			Character->GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(AirAttackTags);
		}
	}
}
