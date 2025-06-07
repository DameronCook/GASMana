// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/GE_ManaPlayerGrounded.h"
#include "GameplayTagContainer.h"

UGE_ManaPlayerGrounded::UGE_ManaPlayerGrounded()
{
	//This will grant the tag to the owner while the effect is active
	InheritableOwnedTagsContainer.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsGrounded")));

	DurationPolicy = EGameplayEffectDurationType::Infinite;
}
