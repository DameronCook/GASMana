// Fill out your copyright notice in the Description page of Project Settings.


#include "Notify/AN_CamShake.h"

#include "PlayerManaCharacter.h"

void UAN_CamShake::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	// ReSharper disable once CppDeprecatedEntity
	//Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	APlayerManaCharacter* Character = Cast<APlayerManaCharacter>(MeshComp->GetOwner());

	if (Character)
	{
		CameraShakeInstance = Character->ActivateCamShake(CameraShake);
	}
}

void UAN_CamShake::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	// ReSharper disable once CppDeprecatedEntity
	//Super::NotifyEnd(MeshComp, Animation);
	
	APlayerManaCharacter* Character = Cast<APlayerManaCharacter>(MeshComp->GetOwner());

	if (Character)
	{
		Character->EndCameraShake(CameraShakeInstance);
	}
}
