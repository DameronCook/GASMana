// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AC_HookShot.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "PlayerManaCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Actors/ManaHookParent.h"
#include "Camera/CameraComponent.h"
#include "CableActor.h"
#include "CableComponent.h"
#include "Actors/GrappleHook.h"
#include "Ability/GA_ManaPlayerZipToPoint.h"
#include "Ability/GA_ManaPlayerSwing.h"



// Sets default values for this component's properties
UAC_HookShot::UAC_HookShot()
{
	PrimaryComponentTick.bCanEverTick = true;

	GrappleState = EGrappleState::E_Inactive;
	MaxGrappleDistance = 2000.f;
	PrimaryComponentTick.TickGroup = ETickingGroup::TG_PrePhysics;
}

void UAC_HookShot::BeginPlay()
{
	Super::BeginPlay();
}

void UAC_HookShot::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	switch (GrappleState)
	{
	case EGrappleState::E_Inactive:
		Inactive();
		break;
	case EGrappleState::E_Firing:
		Firing();
		break;
	case EGrappleState::E_NearTarget:
		NearTarget();
		break;
	case EGrappleState::E_ZipToPointTarget:
		ZipToPointTarget(DeltaTime);
		break;
	case EGrappleState::E_SwingTarget:
		SwingTarget(DeltaTime);
		break;
	default:
		break;
	}

}

void UAC_HookShot::Inactive()
{
	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(GetOwner());
	//GEngine->AddOnScreenDebugMessage(10, 0.1f, FColor::Black, TEXT("PlayerCharacter Class: %s"));


	if (PlayerCharacter)
	{
		UObject* WorldContextObject = GetWorld();

		FVector SpherePos = PlayerCharacter->GetActorLocation();

		float SphereRadius = MaxGrappleDistance;

		// Set what actors to seek out from it's collision channel
		TArray<TEnumAsByte<EObjectTypeQuery>> TraceObjectTypes;
		TraceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldStatic));

		TSubclassOf<AActor> SeekClass = ParentHook;

		TArray<AActor*> ActorsToIgnore;

		TArray<class AActor*> OutActors;

		UKismetSystemLibrary::SphereOverlapActors(WorldContextObject, SpherePos, SphereRadius, TraceObjectTypes, SeekClass, ActorsToIgnore, OutActors);

		//GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Red, FString::Printf(TEXT("Num of Objects : %f"), OutActors.Num()));

		//DrawDebugSphere(GetWorld(), SpherePos, SphereRadius, 12, FColor::Red);

		FindAndSetBestTarget(OutActors, PlayerCharacter);
	}
}

void UAC_HookShot::FindAndSetBestTarget(TArray<AActor*, FDefaultAllocator>& OutActors, APlayerManaCharacter* PlayerCharacter)
{
	float BestAngle = 0.f;
	AManaHookParent* BestTarget = nullptr;

	for (AActor* overlappedActor : OutActors) {
		UE_LOG(LogTemp, Log, TEXT("OverlappedActor: %s"), *overlappedActor->GetName());
		AManaHookParent* CurrentTargetLocal = Cast<AManaHookParent>(overlappedActor);

		if (IsValid(CurrentTargetLocal))
		{
			bool bHitHook = DrawLineToTarget(PlayerCharacter, CurrentTargetLocal);
			if (bHitHook)
			{
				float CurrentAngle = CalculateAngleToTarget(PlayerCharacter, overlappedActor);
				if (CurrentAngle < BestAngle || BestTarget == nullptr )
				{
					BestAngle = CurrentAngle;
					BestTarget = CurrentTargetLocal;
				}
			}
		}
	}
	SetCurrentTarget(BestTarget);
}

void UAC_HookShot::SetCurrentTarget(AManaHookParent* Hook)
{
	if (Hook)
	{
		if (IsValid(CurrentTarget))
		{
			CurrentTarget->SetActive(false);
		}
		CurrentTarget = Hook;
		CurrentTarget->SetActive(true);
	}
}

void UAC_HookShot::Firing()
{
	if (GrappleHook && GrappleHook->DistanceToTarget() < StartJumpDistance)
	{
		HitTarget = true;
		GrappleState = EGrappleState::E_NearTarget;

		APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(GetOwner());
		if (PlayerCharacter)
		{
			
		}
	}
}

void UAC_HookShot::NearTarget()
{
	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(GetOwner());
	if (PlayerCharacter)
	{
		UCharacterMovementComponent* CharMove = PlayerCharacter->GetCharacterMovement();
		if (CurrentTarget && CharMove)
		{
			FVector InitDirection = CurrentTarget->GetCharacterTargetLocation() - PlayerCharacter->GetActorLocation();
			CharacterInitDirection = InitDirection.GetSafeNormal();

			//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString("I'm now on Near Target State!!!"));

			PlayerCharacter->GetHookAbility()->EndHook();

			switch (CurrentTarget->GetGrappleType())
			{
			case EGrappleType::E_ZipToPoint:
				GrappleState = EGrappleState::E_ZipToPointTarget;
				break;
			case EGrappleType::E_Swing:
				GrappleState = EGrappleState::E_SwingTarget;
				CharMove->MaxWalkSpeed = 1600.f;
				CharMove->GravityScale = 3.f;
				OptimalSwingPoint = FindOptimalSwingPoint(PlayerCharacter);
				SwingTargetLocation = CurrentTarget->GetActorLocation();
				break;
			default:
				GrappleState = EGrappleState::E_ZipToPointTarget;
				break;
			}
		}
	}
}

void UAC_HookShot::ZipToPointTarget(float DeltaTime)
{
	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(GetOwner());

	FHitResult OutHit;

	if (PlayerCharacter)
	{
		// Capsule parameters
		const FVector CapsuleLocation = PlayerCharacter->GetActorLocation();
		const float CapsuleRadius = 48.0f;
		const float CapsuleHalfHeight = 85.0f;
		const FQuat CapsuleRotation = FQuat::Identity;

		//Trace Parameters
		bool bTraceComplex = false;
		FCollisionQueryParams QueryParams;

		// Draw the debug capsule
		DrawDebugCapsule(GetWorld(), CapsuleLocation, CapsuleHalfHeight, CapsuleRadius, CapsuleRotation, FColor::Red, false, 0.f);

		bool bHit = GetWorld()->SweepSingleByChannel(OutHit, CapsuleLocation, CapsuleLocation, CapsuleRotation, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight), QueryParams);


		if (CurrentTarget)
		{
			if (bHit || PlayerCharacter->GetDistanceTo(CurrentTarget) < EndJumpDistance)
			{
				EndGrapple();

				if (bHit)
				{
					UGA_ManaPlayerZipToPoint* ZipAbility = PlayerCharacter->GetZipAbility();
					if (ZipAbility)
					{
						ZipAbility->CancelMotionTask();

					}
				}
			}
			else
			{
				PlayerCharacter->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(PlayerCharacter->GetZipToPointTag(), true);

				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString("I'm now on Zip To Point Target State!!!"));
			}


			FVector DirectionToTarget = (CurrentTarget->GetActorLocation() - PlayerCharacter->GetActorLocation()).GetSafeNormal();
			FRotator AngleToTarget = DirectionToTarget.Rotation();

			PlayerCharacter->SetActorRotation(FMath::RInterpTo(PlayerCharacter->GetActorRotation(), AngleToTarget, DeltaTime, 7.f));
		}
	}
}

void UAC_HookShot::SwingTarget(float DeltaTime)
{
	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(GetOwner());
	
	if (PlayerCharacter && CurrentTarget)
	{
		PlayerCharacter->GetAbilitySystemComponent()->TryActivateAbilitiesByTag(PlayerCharacter->GetSwingTag(), true);

		FHitResult OutHit;

		// Capsule parameters
		const FVector CapsuleLocation = PlayerCharacter->GetActorLocation();
		const float CapsuleRadius = 48.0f;
		const float CapsuleHalfHeight = 95.f;
		const FQuat CapsuleRotation = FQuat::Identity;

		//Trace Parameters
		bool bTraceComplex = false;
		FCollisionQueryParams QueryParams;
		// Draw the debug capsule
		//DrawDebugCapsule(GetWorld(), CapsuleLocation, CapsuleHalfHeight, CapsuleRadius, CapsuleRotation, FColor::Red, false, 0.f);

		bool bHit = GetWorld()->SweepSingleByChannel(OutHit, CapsuleLocation, CapsuleLocation, CapsuleRotation, ECollisionChannel::ECC_Visibility, FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight), QueryParams);
	
		if (bHit)
		{
			EndGrapple();
			if (PlayerCharacter->GetSwingAbility())
			{
				PlayerCharacter->GetSwingAbility()->CancelSwing();
			}
		}

		UCharacterMovementComponent* CharMove = PlayerCharacter->GetCharacterMovement();
		if (CharMove)
		{
			//First, let's find the swing arc force

			FVector SwingTargetDir = PlayerCharacter->GetActorLocation() - OptimalSwingPoint;
			float SwingTargetDist = SwingTargetDir.Length();
			float ReductionForce = UKismetMathLibrary::FClamp((8000.f / SwingTargetDist), 1.f, 4.f); 	//We need to calculate how much to reduce this force as the swing continues though depending on How far we are from the target
			FVector SwingArcForce = FindSwingArcForce(PlayerCharacter, 400.f, 2000.f, ReductionForce);		


			//Next, if the player is along the bottom of the swing arc, speed them up a bit. This will prevennt them from staying on the ground too much
			FVector BottomSwingVelocityBoost = FVector::ZeroVector;
			FVector SmallUpwardForce = FVector::ZeroVector;
			if (SwingAngle > -20.f && SwingAngle < 20.f)
			{
				BottomSwingVelocityBoost = PlayerCharacter->GetVelocity().GetSafeNormal() * 60000.f;
				//GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Red, "I should be BOOOSTING THE PLAYER");

				/*  I don't like how this feels I think
				//Finally, Add a small upward force to the player. Only when the player is along the bottom of their arc. This *should* counteract the slow lengthening of the hook that can result in the player colliding with the ground
				SmallUpwardForce = FVector(0.f, 0.f, 50000.f);
				*/
			}

			//Also, Speed them up in the direction the camera is facing. This will keep the player always moving forward! The camera's rotation is kinda locked to the swing... might need to change that
			UCameraComponent* Cam = PlayerCharacter->GetFollowCamera(); 
			FVector CamForwardAdder = FVector::ZeroVector;
			if (Cam)
			{
				FVector CamForward = Cam->GetForwardVector();
				CamForwardAdder = FVector(CamForward.X, CamForward.Y, 0.f).GetSafeNormal();
				CamForwardAdder *= 75000.f;
			}

			//Now, We combine the forces to make an ULTIMATE swing force that we add to the character movement component
			FVector SwingMovementForce = SwingArcForce + BottomSwingVelocityBoost + CamForwardAdder + SmallUpwardForce;

			//GEngine->AddOnScreenDebugMessage(3, 0.1f, FColor::Red, SwingMovementForce.ToString());

			CharMove->AddForce(SwingMovementForce); // <---- Okay, maybe we need to multiply by mass? CharMove->Mass


			// Okay cast a line down and whenever the player is just above the ground, offset their position so that they won't hit it at the bottom of a swing
			FHitResult OutLineHit;

			// Line parameters
			const FVector LineStartLocation = PlayerCharacter->GetActorLocation();
			const float LineTraceLength = 300.f;
			const FVector LineEndLocation = PlayerCharacter->GetActorLocation() + (PlayerCharacter->GetActorUpVector() * -LineTraceLength);

			//Trace Parameters
			FCollisionQueryParams LineQueryParams;
			LineQueryParams.AddIgnoredActor(PlayerCharacter);

			//DrawDebugLine(GetWorld(), LineStartLocation, LineEndLocation, FColor::Red);
			bool bBottomLineHit = GetWorld()->LineTraceSingleByChannel(OutLineHit, LineStartLocation, LineEndLocation, ECollisionChannel::ECC_Visibility, LineQueryParams);

			if (bBottomLineHit)
			{
				//GEngine->AddOnScreenDebugMessage(8, 0.1f, FColor::Purple, "I should be OFFSETTING THE PLAYER Z AAAAAAAAAA");
				float AddOffset = UKismetMathLibrary::MapRangeClamped(OutLineHit.Distance, 0.f, LineTraceLength, 25.f, 1.f);

				PlayerCharacter->AddActorWorldOffset(FVector(0.f, 0.f, AddOffset));
			}

			//Also, let's rotate the player in the direction of the swing force
			FRotator CharacterRotation = FindCharacterRotation(PlayerCharacter, DeltaTime);
			PlayerCharacter->SetActorRotation(CharacterRotation);

			FRotator ControlInterp = UKismetMathLibrary::RInterpTo(PlayerCharacter->GetController()->GetControlRotation(), FRotator(CharacterRotation.Pitch, CharacterRotation.Yaw, 0.f), DeltaTime, 4.0f);
			PlayerCharacter->GetController()->SetControlRotation(ControlInterp);

			//Then, we find the new swing angle considering the force of the swing, the direction to the swing point, and the velocity of the player
			SwingAngle = FindSwingAngle(PlayerCharacter);

			//Lastly, we set the velocity of the char movement component to match the direction of the swing. We also clamp it by our min and max speeds here.
			UCapsuleComponent* Capsule =  PlayerCharacter->GetCapsuleComponent();
			if (Capsule)
			{
				FVector DirectionToSwingPoint = Capsule->GetComponentLocation() - CurrentTarget->GetActorLocation();
				FVector DirectionToSwingPointNormal = DirectionToSwingPoint / UKismetMathLibrary::VSize(DirectionToSwingPoint);
				FVector PlayerVelocity = CharMove->Velocity;

				float DirectionVelocityDot = UKismetMathLibrary::Dot_VectorVector(DirectionToSwingPoint, PlayerVelocity);


				if (DirectionVelocityDot >= 0)
				{
					FVector ProjectedVelocity = UKismetMathLibrary::ProjectVectorOnToPlane(PlayerCharacter->GetVelocity(), DirectionToSwingPointNormal);
					FVector ClampedVelocity = UKismetMathLibrary::ClampVectorSize(ProjectedVelocity, -3600, 3600);

					CharMove->Velocity = ClampedVelocity;
					GEngine->AddOnScreenDebugMessage(4, 0.1f, FColor::Orange, CharMove->Velocity.ToString());

				}
			}
		}

		if (PlayerCharacter->GetCachedInputDirection().Size() <= 0.1f)
		{
			if (SwingAngle < 18.f && SwingAngle > -18.f)
			{
				if (PlayerCharacter->GetVelocity().Size() < 8.f)
				{
					EndGrapple();
					if (PlayerCharacter->GetSwingAbility())
					{
						PlayerCharacter->GetSwingAbility()->CancelSwing();
					}
				}
			}
		}
	}
}

bool UAC_HookShot::DrawLineToTarget(ACharacter* Character, AActor* OverlappedActor)
{
	FHitResult OutHitLine;

	if (!OverlappedActor)
	{
		return false;
	}
	// Line parameters
	const FVector LineStartLocation = Character->GetActorLocation();
	const FVector LineEndLocation = OverlappedActor->GetActorLocation() + FVector(0.f, 0.f, 25.f);

	//Trace Parameters
	//bool bLineTraceComplex = false;
	FCollisionQueryParams LineQueryParams;
	LineQueryParams.AddIgnoredActor(Character);
	ECollisionChannel CollisionChannel = ECollisionChannel::ECC_GameTraceChannel2;
	bool bHitAnything = GetWorld()->LineTraceSingleByChannel(OutHitLine, LineStartLocation, LineEndLocation, CollisionChannel, LineQueryParams);

	//DrawDebugLine(GetWorld(), LineStartLocation, LineEndLocation, FColor::Red);

	return OutHitLine.GetActor() == OverlappedActor;
}

float UAC_HookShot::CalculateAngleToTarget(ACharacter* Character, AActor* OverlappedActor)
{
	if (APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(Character))
	{
		UCameraComponent* Camera = PlayerCharacter->GetFollowCamera();
		if (Camera)
		{
			FVector CameraForward = Camera->GetForwardVector();

			FVector DirectionToTarget = OverlappedActor->GetActorLocation() - Character->GetActorLocation();
			DirectionToTarget.Normalize();

			float Angle = acosf(FVector::DotProduct(DirectionToTarget, CameraForward));
			return Angle;
		}
	}
	return 0.0f;
}

void UAC_HookShot::AttemptGrapple()
{
	if (GrappleState == EGrappleState::E_Inactive)
	{
		APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(GetOwner());
		if (PlayerCharacter)
		{
			//First, see if there is a current target.
			if (CurrentTarget)
			{
				FString Name = ("CurrentTargetName" + CurrentTarget->GetName());
				//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Purple, Name);

				FVector Location(PlayerCharacter->GetActorLocation());
				FRotator Rotation(0.0f, 0.0f, 0.0f);
				FTransform HookTransform(Rotation, Location);//PlayerCharacter->GetActorTransform();
				FActorSpawnParameters SpawnInfo;
				SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				//Next, create a grapple hook
				GrappleHook = GetWorld()->SpawnActorDeferred<AGrappleHook>(GrappleHookClass, HookTransform);
				if (GrappleHook)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString("Grapple Hook was created, setting target."));
					GrappleHook->TargetHook = CurrentTarget;
					GrappleHook->GrapplingSpeed = 5000.f;
					GrappleHook->SetActorLocation(Location);
					GrappleHook->FinishSpawning(HookTransform);


					//Then, create a cable attached to the hook
					GrappleCable = GetWorld()->SpawnActorDeferred<ACableActor>(GrappleCableClass, HookTransform); /* This line causes the crash...? */
					if (GrappleCable)
					{
						//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Purple, FString("Grapple Cable was created, setting target."));

						GrappleCable->SetActorLocation(Location);
						GrappleCable->CableComponent->AttachToComponent(PlayerCharacter->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, "hand_rSocket");
						GrappleCable->CableComponent->SetAttachEndTo(GrappleHook, EndComponentName);
						GrappleCable->FinishSpawning(HookTransform);

						//Finally Set the rotation of the player to face the target
						//FVector OwnerToTarget = (CurrentTarget->GetTargetLocation() - PlayerCharacter->GetActorLocation()).GetSafeNormal();
						//PlayerCharacter->SetActorRotation(UKismetMathLibrary::MakeRotFromXZ(FVector::ZeroVector, OwnerToTarget));

						GrappleState = EGrappleState::E_Firing;
					}
				}
			}
			//If the player just so happens to be wall running, launch them away and end the wall run
			if (PlayerCharacter->GetWallRunAbility())
			{
				FVector WallNormal = PlayerCharacter->GetWallRunImpactNormal().GetSafeNormal();
				FVector Forward = PlayerCharacter->GetActorForwardVector().GetSafeNormal();
				FVector Bisector = (WallNormal + Forward).GetSafeNormal();

				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Just checkin' I read this");

				PlayerCharacter->GetWallRunAbility()->OnWallRunFinished();

				PlayerCharacter->LaunchCharacter(Bisector * 500, true, true);
			}
		}
	}
}

void UAC_HookShot::EndGrapple()
{
	SetCurrentTarget(nullptr);
	if (GrappleCable)
	{
		GrappleCable->Destroy();
		GrappleCable = nullptr;
	}
	if (GrappleHook)
	{
		GrappleHook->Destroy();
		GrappleHook = nullptr;
	}
	GrappleState = EGrappleState::E_Inactive;

	APlayerManaCharacter* PlayerCharacter = Cast<APlayerManaCharacter>(GetOwner());

	if (UAbilitySystemComponent* ASC = PlayerCharacter->GetAbilitySystemComponent())
	{
		FGameplayTagContainer Tags;
		Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsZipToPoint")));
		Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsSwinging")));
		Tags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsHooked")));
		ASC->RemoveActiveEffectsWithTags(Tags);
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Orange, FString("I should have removed hooked and zip to point!!!"));
	}

	if (PlayerCharacter->GetHookAbility())
	{
		PlayerCharacter->GetHookAbility()->EndHook();
	}

	UCharacterMovementComponent* CharMove = PlayerCharacter->GetCharacterMovement();

	if (CharMove)
	{
		CharMove->MaxWalkSpeed = 700.f;
		CharMove->GravityScale = 2.f;

	}
}



///////////////////////////////
///	Swing Functions

FVector UAC_HookShot::FindOptimalSwingPoint(APlayerManaCharacter* Character)
{
	if (Character)
	{
		FVector CameraForward = Character->GetFollowCamera()->GetForwardVector();
		FVector ActorLoc = Character->GetActorLocation();
		double ZVelocity = Character->GetVelocity().Z;
		FVector ZIncrease = FVector(0.f, 0.f, 2500.f);

		float ForwardVelocityLength;
		ForwardVelocityLength = UKismetMathLibrary::VSize(FVector(ZVelocity, 0, 0));
		ForwardVelocityLength *= 1.2;
		ForwardVelocityLength = UKismetMathLibrary::FClamp(ForwardVelocityLength, 1000.f, 3000.f);

		FVector CameraAndForwardVelocity = UKismetMathLibrary::Multiply_VectorFloat(CameraForward, ForwardVelocityLength);

		FVector OptimalPoint = ActorLoc + CameraAndForwardVelocity + ZIncrease;
		return OptimalPoint;
	}
	return FVector();
}

FVector UAC_HookShot::FindSwingArcForce(APlayerManaCharacter* Character, float MinVelocity, float MaxVelocity, float ReduceSwingForceFactor)
{
	if (Character)
	{
		FVector ClampedVelocity = UKismetMathLibrary::ClampVectorSize(Character->GetVelocity(), MinVelocity, MaxVelocity);
		FVector SwingTargetDir = Character->GetActorLocation() - SwingTargetLocation;
		float DirectionalDot = UKismetMathLibrary::Dot_VectorVector(ClampedVelocity, SwingTargetDir);
		SwingTargetDir.Normalize();

		FVector SwingForce = UKismetMathLibrary::Multiply_VectorFloat(SwingTargetDir, DirectionalDot);
		SwingForce *= -2.f;
		SwingForce /= ReduceSwingForceFactor;
		return SwingForce;
	}
	return FVector();
}

FVector UAC_HookShot::FindSwingLaunchForce(APlayerManaCharacter* Character, float ForwardSpeed, float UpSpeed)
{
	FVector VelocityDir = Character->GetVelocity().GetSafeNormal();

	float ForwardAngleMultiplier =  60.f / SwingAngle;
	float ForwardVelocityMultiplier = ForwardSpeed * ForwardAngleMultiplier;

	float ClampedSwingAngle = SwingAngle;
	ClampedSwingAngle = UKismetMathLibrary::FClamp(ClampedSwingAngle, 5.f, 45.f);
	float UpVelocityMultiplier = UpSpeed * ClampedSwingAngle;

	FVector LaunchDir = UKismetMathLibrary::Multiply_VectorFloat(VelocityDir, ForwardVelocityMultiplier);
	LaunchDir = UKismetMathLibrary::Add_VectorVector(LaunchDir, FVector(0.f, 0.f, UpVelocityMultiplier));
	LaunchDir = UKismetMathLibrary::Divide_VectorFloat(LaunchDir, 1500.f);

	float VelocityLength = UKismetMathLibrary::FClamp(UKismetMathLibrary::VSize(Character->GetVelocity()), 1000.f, 2000.f);

	LaunchDir = UKismetMathLibrary::Multiply_VectorFloat(LaunchDir, VelocityLength);
	

	return LaunchDir;
}

float UAC_HookShot::FindSwingAngle(APlayerManaCharacter* Character)
{
	if (Character)
	{
		FVector DirectionToPlayer = (OptimalSwingPoint - Character->GetActorLocation());
		FVector PlayerVelocity = Character->GetVelocity();
		FVector CrossProduct = FVector::CrossProduct(DirectionToPlayer, PlayerVelocity);

		FRotator SwingRot = UKismetMathLibrary::MakeRotFromZX(DirectionToPlayer, CrossProduct * -1);

		GEngine->AddOnScreenDebugMessage(2, 0.1f, FColor::Orange, FString::Printf(TEXT("Calculating Swing Angle: %f"), SwingRot.Roll)); 

		return SwingRot.Roll;
	}
	return 0.0f;
}

FRotator UAC_HookShot::FindCharacterRotation(APlayerManaCharacter* Character, float DeltaTime)
{
	if (Character)
	{
		FRotator RotationFromVelocity = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
		FRotator SwingSideAngle = FindSwingSideAngle(Character);
		FRotator TargetRotation = FRotator(SwingSideAngle.Pitch, RotationFromVelocity.Yaw, SwingSideAngle.Roll);

		FRotator InterpToRotation = UKismetMathLibrary::RInterpTo(Character->GetActorRotation(), TargetRotation, DeltaTime, 2.f);

		return InterpToRotation;
	}

	return FRotator();
}

FRotator UAC_HookShot::FindSwingSideAngle(APlayerManaCharacter* Character)
{
	if (Character)
	{
		FVector DirectionToPlayer = (OptimalSwingPoint - Character->GetActorLocation()).GetSafeNormal();
		FVector PlayerVelocity = Character->GetVelocity().GetSafeNormal();

		FVector CrossProduct = UKismetMathLibrary::Cross_VectorVector(PlayerVelocity, DirectionToPlayer) * -1;

		FRotator SwingSideAngle = UKismetMathLibrary::MakeRotFromZY(DirectionToPlayer, CrossProduct);
		
		return SwingSideAngle;
	}
	return FRotator();
}


