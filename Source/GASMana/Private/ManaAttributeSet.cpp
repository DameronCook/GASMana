// Fill out your copyright notice in the Description page of Project Settings.


#include "ManaAttributeSet.h"
#include "Net/UnrealNetwork.h"

UManaAttributeSet::UManaAttributeSet()
{

}

void UManaAttributeSet::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	DOREPLIFETIME_CONDITION_NOTIFY(UManaAttributeSet, Health, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UManaAttributeSet, Stamina, COND_None, REPNOTIFY_Always)
	DOREPLIFETIME_CONDITION_NOTIFY(UManaAttributeSet, Mana, COND_None, REPNOTIFY_Always)
}

void UManaAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UManaAttributeSet, Health, OldHealth);
}

void UManaAttributeSet::OnRep_Stamina(const FGameplayAttributeData& OldStamina)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UManaAttributeSet, Stamina, OldStamina);
}

void UManaAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UManaAttributeSet, Mana, OldMana);
}
