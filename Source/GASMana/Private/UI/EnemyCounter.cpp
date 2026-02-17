// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/EnemyCounter.h"

#include "Components/TextBlock.h"

void UEnemyCounter::SetCounterText(FText Text)
{
	if (EnemyCounterText)
	{
		EnemyCounterText->SetText(Text);
	}
}
