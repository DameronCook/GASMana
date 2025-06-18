// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/GE_ManaPlayerAirborne.h"
#include "GameplayTagContainer.h"

UGE_ManaPlayerAirborne::UGE_ManaPlayerAirborne()
{
	//This will grant the tag to the owner while the effect is active
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));

	DurationPolicy = EGameplayEffectDurationType::Infinite;
}
