// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerManaCharacter.h"
#include "ManaPlayerAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Effect/GE_ManaPlayerGrounded.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Actors/BaseManaEnemy.h"

APlayerManaCharacter::APlayerManaCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(35.f, 90.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;


	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 360.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void APlayerManaCharacter::BeginPlay()
{
	Super::BeginPlay();

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();

	if (AbilitySystem && GroundedEffectClass && FreeEffectClass)
	{
			AbilitySystem->ApplyGameplayEffectToSelf(GroundedEffectClass->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystem->MakeEffectContext());
			AbilitySystem->ApplyGameplayEffectToSelf(FreeEffectClass->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystem->MakeEffectContext());
	}

	if (AbilitySystem)
	{
		AbilitySystem->RegisterGameplayTagEvent(
			FGameplayTag::RequestGameplayTag(FName("Player.IsBlocking")),
			EGameplayTagEventType::NewOrRemoved
		).AddUObject(this, &APlayerManaCharacter::OnBlockingTagChanged);
	}

	AddEquipment(FName("hand_rSocket"), GetRightHandEquipment());
	AddEquipment(FName("hand_lSocket"), GetLeftHandEquipment());

	if (PlayerHUDClass)
	{
		//Configure Player HUD
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		PlayerHUD = CreateWidget<UUserWidget>(PlayerController, PlayerHUDClass);
		PlayerHUD->AddToViewport();
	}

	UpdateStaminaRegen();
}

void APlayerManaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();
	float CurrentMana = AbilitySystem->GetNumericAttribute(UManaAttributeSet::GetManaAttribute());

	if (CurrentMana > 0)
	{
		if (AbilitySystem && AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne"))) && !AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning"))))
		{
			if (WallRunCheck())
			{
				AbilitySystem->TryActivateAbilitiesByTag(WallRunTagContainer, true);
			}
		}
	}

	if (ActiveWallRunAbility)
	{
		UpdateWallRunVertical(DeltaTime);



		FVector NewWallRunHorizontalDirection = UpdateWallRunHorizontal();
		GetCharacterMovement()->Velocity.X = NewWallRunHorizontalDirection.X * WallRunStrength;
		GetCharacterMovement()->Velocity.Y = NewWallRunHorizontalDirection.Y * WallRunStrength;

		SetActorRotation(FMath::RInterpTo(GetActorRotation(), NewWallRunHorizontalDirection.Rotation(), DeltaTime, 5.0f));


		if (CurrentMana <= 0.0f)
		{
			ActiveWallRunAbility->OnWallRunFinished();
		}
	}
}

//////////////////////////////////////////////////////////////////////////
// Movement

void APlayerManaCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();

	if (AbilitySystem && GroundedEffectClass)
	{
		AbilitySystem->ApplyGameplayEffectToSelf(GroundedEffectClass->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystem->MakeEffectContext());
	}

	//Remove the grounded effect by tag
	FGameplayTagContainer AirborneTags;
	AirborneTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")));
	AirborneTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning")));
	AbilitySystem->RemoveActiveEffectsWithGrantedTags(AirborneTags);

	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Landed");
	//}
}

void APlayerManaCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PrevCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PrevCustomMode);

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();

	if (!AbilitySystem)
	{
		return;
	}

	//If now falling, apply the airborne effect

	if (GetCharacterMovement()->IsFalling())
	{
		if (AirborneEffectClass)
		{
			AbilitySystem->ApplyGameplayEffectToSelf(AirborneEffectClass->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystem->MakeEffectContext());
		}
	}
	//If needed, remove the airborne effect here. However, the landed function should be able to handle this
}

//////////////////////////////////////////////////////////////////////////
// Wall Run functions... move to actor component???

bool APlayerManaCharacter::WallRunCheck()
{
	FHitResult OutHit;

	// Capsule parameters
	const FVector CapsuleLocation = GetActorLocation();
	const float CapsuleRadius = 40.0f;
	const float CapsuleHalfHeight = 40.0f;
	const FQuat CapsuleRotation = FQuat::Identity;

	//Trace Parameters
	bool bTraceComplex = false;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	// Draw the debug capsule
	//DrawDebugCapsule(GetWorld(), CapsuleLocation, CapsuleHalfHeight, CapsuleRadius, CapsuleRotation, FColor::Red, false, 0.f);

	bool bHit = GetWorld()->SweepSingleByChannel(OutHit, CapsuleLocation, CapsuleLocation, CapsuleRotation,ECollisionChannel::ECC_Visibility, FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight), QueryParams);

	if (bHit)
	{
		//DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 20, FColor::Green, false, 1.0f);
		if (CanWallRunOnSurface(OutHit.ImpactNormal))
		{
			//Initialize variables based on side of wall here
			if (IsWallRunningAlongRightSide(OutHit.ImpactNormal))
			{
				WallRunSide = EWallRunSide::Right;
				WallRunDir = FVector(0, 0, -1);
				WallRunDirection = SetWallRunDirection(WallRunDir, OutHit.ImpactNormal);
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Wall Run along Right!");
				}
				PlayableWallRunMontage = WallRunRightMontage;
			}
			else
			{
				WallRunSide = EWallRunSide::Left;
				WallRunDir = FVector(0, 0, 1);
				WallRunDirection = SetWallRunDirection(WallRunDir, OutHit.ImpactNormal);
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "Wall Run along Left!");
				}
				PlayableWallRunMontage = WallRunLeftMontage;
			}

			//Set the variables that need to be reset to start the vertical motion
			WallRunElapsedTime = 0.0f;
			WallRunStartZ = GetActorLocation().Z;

			//Start the wall run
			return true;
		}
	}
	return false;
}

bool APlayerManaCharacter::CanWallRunOnSurface(FVector ImpactNormal)
{
	//Calculate if the wall we're on is angled down like an overhang 
	if (ImpactNormal.Z < -.09)
	{
		return false;
	}

	//Calculate Surface Angle  of the impact point
	FVector XYVector = FVector(ImpactNormal.X, ImpactNormal.Y, 0);				//Gives us the XY Plane of the surface normal. Basically it gives us the horizontal plane of the point
	XYVector.Normalize();														//Enures it's a unit vector		
	double FloorAngle = acos(XYVector.Dot(ImpactNormal));						//Gives the angle in radians between the two vectors. So the Dot product returns how different the two planes are. 
	FMath::RadiansToDegrees(FloorAngle);										//Convert to degrees for later implementation
	
	//Calculate Angle walkable by the floor
	double WalkableFloorAngle = GetCharacterMovement()->GetWalkableFloorAngle();

	return FloorAngle < WalkableFloorAngle;
}

bool APlayerManaCharacter::IsWallRunningAlongRightSide(FVector ImpactNormal)
{
	FVector ActorRight = GetActorRightVector();
	double RightVectorDot = ImpactNormal.Dot(ActorRight);

	if (RightVectorDot < 0)
	{
		return true;
	}

	return false;
}

FVector APlayerManaCharacter::SetWallRunDirection(FVector SideMultiplier, FVector ImpactNormal)
{
	return FVector::CrossProduct(ImpactNormal, SideMultiplier);
}

FVector APlayerManaCharacter::UpdateWallRunHorizontal()
{
	FHitResult OutHit;

	// Capsule parameters
	const FVector LineStartLocation = GetActorLocation();
	const float LineTraceLength = 200.0f;
	const FVector LineEndLocation = (FVector::CrossProduct(WallRunDirection, WallRunDir) * LineTraceLength) + LineStartLocation;

	//Trace Parameters
	bool bTraceComplex = false;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	//DrawDebugLine(GetWorld(), LineStartLocation, LineEndLocation, FColor::Red);
	bool bHit = GetWorld()->LineTraceSingleByChannel(OutHit, LineStartLocation, LineEndLocation, ECollisionChannel::ECC_Visibility, QueryParams);

	if (bHit)
	{
		WallRunDirection = SetWallRunDirection(WallRunDir, OutHit.ImpactNormal);
		return WallRunDirection.GetSafeNormal();
	}
	else
	{
		if (ActiveWallRunAbility)
		{
			ActiveWallRunAbility->OnWallRunFinished();
		}
		GetCharacterMovement()->Velocity.Set(WallRunDirection.X * 800, WallRunDirection.Z * 800, 0.0f);
		return FVector();
	}
}

void APlayerManaCharacter::UpdateWallRunVertical(float DeltaTime)
{
	WallRunElapsedTime += DeltaTime;
	float Time = WallRunElapsedTime / WallRunDuration;
	if (Time >= 1.0f)
	{
		if (ActiveWallRunAbility)
		{
			ActiveWallRunAbility->OnWallRunFinished();
			return;
		}
	}

	float SineOffset = WallRunAmplitude * FMath::Sin(2 * PI * WallRunFreq * Time);

	FVector CurrentLocation = GetActorLocation();
	CurrentLocation.Z = WallRunStartZ + SineOffset;
	SetActorLocation(CurrentLocation, true);
}

//////////////////////////////////////////////////////////////////////////
// Combat

void APlayerManaCharacter::Blocking()
{
	Super::Blocking();

	if (UManaPlayerAnimInstance* AnimInstance = Cast<UManaPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance->SetIsBlocking(true);
	}
}

void APlayerManaCharacter::FinishedBlocking()
{
	Super::FinishedBlocking();

	if (UManaPlayerAnimInstance* AnimInstance = Cast<UManaPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance->SetIsBlocking(false);
	}
}

void APlayerManaCharacter::HandleMelee()
{
	Super::HandleMelee();

	//if (GEngine && GetCharacterMovement()->IsFalling() == false) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "AttackSWING");
	//}

	FVector Position = GetMesh()->GetSocketLocation(FName("hand_rSocket"));
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	UClass* Enemy = BaseEnemy;
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(this);
	TArray<AActor*> OutActors;

	bool bHit = UKismetSystemLibrary::SphereOverlapActors(GetWorld(), Position, 40, ObjectTypes, Enemy, IgnoreActors, OutActors);

	if (bHit)
	{
		for (AActor* HitActor : OutActors)
		{
			ABaseManaEnemy* HitManaCharacter = Cast<ABaseManaEnemy>(HitActor);
			if (HitManaCharacter)
			{
				UAbilitySystemComponent* TargetAbilitySystemComponent = HitManaCharacter->GetAbilitySystemComponent();
				if (TargetAbilitySystemComponent)
				{
					FGameplayTag EventTag = FGameplayTag::RequestGameplayTag(FName("Character.Damaged"));
					FGameplayEventData EventData;
					EventData.Instigator = this;
					EventData.Target = HitManaCharacter;
					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(this, EventTag, EventData);
					HitManaCharacter->ShowHealth();
				}
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////////
//Implementation

float APlayerManaCharacter::GetHealth_Implementation() const
{
	return GetAbilitySystemComponent()->GetNumericAttribute(UManaAttributeSet::GetHealthAttribute());
}

float APlayerManaCharacter::GetHealthAsRatio_Implementation() const
{
	return GetHealth_Implementation()/ GetAbilitySystemComponent()->GetNumericAttribute(UManaAttributeSet::GetMaxHealthAttribute());
}

float APlayerManaCharacter::GetStamina_Implementation() const
{
	return GetAbilitySystemComponent()->GetNumericAttribute(UManaAttributeSet::GetStaminaAttribute());
}

float APlayerManaCharacter::GetStaminaAsRatio_Implementation() const
{
	return GetStamina_Implementation()/GetAbilitySystemComponent()->GetNumericAttribute(UManaAttributeSet::GetMaxStaminaAttribute());
}

float APlayerManaCharacter::GetMana_Implementation() const
{
	return GetAbilitySystemComponent()->GetNumericAttribute(UManaAttributeSet::GetManaAttribute());;
}

float APlayerManaCharacter::GetManaAsRatio_Implementation() const
{
	return GetMana_Implementation()/ GetAbilitySystemComponent()->GetNumericAttribute(UManaAttributeSet::GetMaxManaAttribute());
}

void APlayerManaCharacter::OnBlockingTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    // Tag was removed if NewCount == 0
    if (NewCount == 0)
    {
        FinishedBlocking();
    }
}


//////////////////////////////////////////////////////////////////////////
// Input

void APlayerManaCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void APlayerManaCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &APlayerManaCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Attacking
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerManaCharacter::Attack);

		// Rolling
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this, &APlayerManaCharacter::Roll);

		// Blocking
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Ongoing, this, &APlayerManaCharacter::Block);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &APlayerManaCharacter::StopBlock);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerManaCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &APlayerManaCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerManaCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerManaCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APlayerManaCharacter::Jump()
{
	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(JumpTagContainer, true);

	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Jump!");
	//}
}

void APlayerManaCharacter::Move(const FInputActionValue& Value)
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();
	bool IsFree = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.IsFree")));
	
	if (AbilitySystem)
	{
		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();

		// Add or remove the running tag based on input
		FGameplayTag RunningTag = FGameplayTag::RequestGameplayTag(FName("Character.IsRunning"));
		UManaPlayerAnimInstance* AnimInstance = Cast<UManaPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		if (!MovementVector.IsNearlyZero())
		{
			if (!AbilitySystem->HasMatchingGameplayTag(RunningTag))
			{
				AbilitySystem->AddLooseGameplayTag(RunningTag);
			}
			if (AnimInstance)
			{
				AnimInstance->SetIsRunning(true);
			}
		}
		else
		{
			if (AbilitySystem->HasMatchingGameplayTag(RunningTag))
			{
				AbilitySystem->RemoveLooseGameplayTag(RunningTag);
			}
			if (AnimInstance)
			{
				AnimInstance->SetIsRunning(false);
			}
		}

		if (Controller != nullptr)
		{
			// find out which way is forward
			const FRotator Rotation = Controller->GetControlRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);

			// get forward vector
			const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

			// get right vector 
			const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

			CachedInputDirection = (ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X).GetSafeNormal();

			if (IsFree)
			{
				// add movement 
				AddMovementInput(ForwardDirection, MovementVector.Y);
				AddMovementInput(RightDirection, MovementVector.X);
			}
		}
	}
}

void APlayerManaCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerManaCharacter::Attack(const FInputActionValue& Value)
{
	if (GetAbilitySystemComponent()->TryActivateAbilitiesByTag(AttackTagContainer, true))
	{
		UManaPlayerAnimInstance* AnimInstance = Cast<UManaPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		AnimInstance->SetIsAttacking(true);
	}
}

void APlayerManaCharacter::Block(const FInputActionValue& Value)
{
	//if (GEngine && GetCharacterMovement()->IsFalling() == false) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Block");
	//}

	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(BlockTagContainer, true);
}

void APlayerManaCharacter::StopBlock(const FInputActionValue& Value)
{
	//if (GEngine && GetCharacterMovement()->IsFalling() == false) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "StopBlock");
	//}

	FGameplayTagContainer Tag;
	Tag.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsBlocking")));
	GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(Tag);

	UpdateStaminaRegen();
}

void APlayerManaCharacter::Roll(const FInputActionValue& Value)
{
	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(RollTagContainer, true);
}


//////////////////////////////////////////////////////////////////////////
// Ability Regen
void APlayerManaCharacter::UpdateStaminaRegen()
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();
	if (!AbilitySystem|| !StaminaRegenBlockEffectClass || !StaminaRegenEffectClass)
		return;

	//Check these to see if the player should recharge stamina or not
	bool bIsBlocking = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsBlocking")));
	bool bIsAttacking = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAttacking")));
	bool bIsRolling = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsRolling")));

	// Remove both regen effects first
	AbilitySystem->RemoveActiveGameplayEffectBySourceEffect(StaminaRegenEffectClass, AbilitySystem);
	AbilitySystem->RemoveActiveGameplayEffectBySourceEffect(StaminaRegenBlockEffectClass, AbilitySystem);

	if (bIsAttacking || bIsRolling)
	{
		return;
	}

	//bool bIsFree = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.IsFree")));

	if (bIsBlocking)
	{
		AbilitySystem->ApplyGameplayEffectToSelf(StaminaRegenBlockEffectClass->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystem->MakeEffectContext());
	}
	else
	{
		AbilitySystem->ApplyGameplayEffectToSelf(StaminaRegenEffectClass->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystem->MakeEffectContext());
	}
}
