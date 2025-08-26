// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorator/BTDecorator_IsAlive.h"

#include "GASMana/GASManaCharacter.h"

bool UBTDecorator_IsAlive::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	Super::CalculateRawConditionValue(OwnerComp, NodeMemory);


	//Add functionality to check if the character's Health is > 0 here. For now return true.
	return true;
}
