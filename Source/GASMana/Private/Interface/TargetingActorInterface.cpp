// Fill out your copyright notice in the Description page of Project Settings.


#include "Interface/TargetingActorInterface.h"

// Add default functionality here for any ITargetingActorInterface functions that are not pure virtual.
bool ITargetingActorInterface::IsSelectingTarget() const { return false; }
bool ITargetingActorInterface::SelectTarget(bool) { return false; }
AActor* ITargetingActorInterface::GetCurrentTarget() const { return nullptr; }
AActor* ITargetingActorInterface::SetCurrentTarget(AActor*) { return nullptr; }
FVector2D ITargetingActorInterface::GetCurrentTargetSelectionInput() const { return FVector2D::ZeroVector; }
