// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerManaCharacter.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ManaPlayerAnimInstance.h"
#include "ManaPlayerController.h"
#include "Ability/GA_ManaPlayerAirAttack.h"
#include "Ability/GA_ManaPlayerAttack.h"
#include "Ability/GA_ManaPlayerFocus.h"
#include "Actors/BaseManaEnemy.h"
#include "Actors/ManaHookParent.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Camera/ManaSpringArmComponent.h"
#include "Components/AC_HookShot.h"
#include "Components/AC_WallRun.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Effect/GE_ManaPlayerGrounded.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Item/LeftHandEquipment.h"
#include "Item/RightHandEquipment.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "UI/FadeOutScreen.h"
#include "UI/StackedProgressBar.h"
#include "UI/StaminaBar.h"


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
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<UManaSpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Using the camera boom, add lag to create a more reactive camera
	CameraBoom->bEnableCameraRotationLag = true;
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->CameraRotationLagSpeed = 8.f;
	CameraBoom->CameraLagSpeed = 5.f;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Create a hook shot component
	HookShotComponent = CreateDefaultSubobject<UAC_HookShot>(TEXT("HookShotComponent"));

	//Create a hook shot component
	WallRunComponent = CreateDefaultSubobject<UAC_WallRun>(TEXT("WallRunComponent"));

	//Create a sphere mesh that checks for enemies to load them in
	LoaderSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LoaderSphere"));
	LoaderSphere->SetupAttachment(RootComponent);
	LoaderSphere->SetSphereRadius(3000.f, false);
	LoaderSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	LoaderSphere->SetCollisionObjectType(ECC_WorldDynamic);
	LoaderSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	//Blocking
	CurrentBlockingMontage = ShieldBlockMontage; //In the future, set this based on cur equipment

	StaminaBarWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StamBarWidget"));
	StaminaBarWidgetComponent->SetWidgetClass(StamBarClass);
	StaminaBarWidgetComponent->SetupAttachment(RootComponent);
}

void APlayerManaCharacter::OnLoaderSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		if (AGASManaCharacter* Character = Cast<AGASManaCharacter>(OtherActor))
		{
			if (Character != this)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, "Found A GASManaCharacter:" + Character->GetName());
				Character->LoadMe();
			}
		}
}

void APlayerManaCharacter::OnLoaderEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (AGASManaCharacter* Character = Cast<AGASManaCharacter>(OtherActor))
	{
		if (Character != this)
		{
			Character->UnloadMe();
		}
	}
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

	EquipmentState = EEquipmentState::EES_Unequipped;
	
	if (APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		if (PlayerHUDClass)
		{
			//Configure Player HUD
			PlayerHUD = CreateWidget<UUserWidget>(PlayerController, PlayerHUDClass);
			PlayerHUD->AddToViewport();
		}
		
	}

	if (LoaderSphere)
	{
		LoaderSphere->OnComponentBeginOverlap.AddDynamic(this, &APlayerManaCharacter::OnLoaderSphereOverlap);
		LoaderSphere->OnComponentEndOverlap.AddDynamic(this, &APlayerManaCharacter::OnLoaderEndOverlap);
	}

	ManaPlayerAnimInstance = Cast<UManaPlayerAnimInstance>(GetMesh()->GetAnimInstance());

	if (StaminaBarWidgetComponent)
	{
		StaminaBarInstance = Cast<UStaminaBar>(StaminaBarWidgetComponent->GetWidget());
		if (StaminaBarInstance)
		{
			StaminaBarInstance->BindToAnimationFinished(StaminaBarInstance->GetEndAnimation(), StaminaBarInstance->GetFadeOutAnimEndedDelegate());
			StaminaBarInstance->PlayAnimation(StaminaBarInstance->GetEndAnimation());
			bAlreadyShowing = false;
		}
	}

	UpdateStaminaRegen();

}

void APlayerManaCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateManaRegen();

	UpdateFocusedCamera(DeltaTime);
	
	UpdateBlockingState();

	UpdateStaminaBar(DeltaTime);

	if (CurrentJumpTimer > 0.f)
	{
		CountDownJumpTimer(DeltaTime);
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

	if (ActiveAirAttackAbility)
	{
		ActiveAirAttackAbility->OnJumpLanded();
	}
	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Landed");
	//}
}

void APlayerManaCharacter::OnMovementModeChanged(const EMovementMode PrevMovementMode, const uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

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

FVector APlayerManaCharacter::GamepadRightSwingForce(const float MovementInput) const
{
	float VelocitySize = GetVelocity().Size();
	VelocitySize = UKismetMathLibrary::FClamp(VelocitySize, 0.f, 1000.f);

	const float ReduceSwingForceVelocity = 50.f / SwingSpeedBalancer;

	const float FinalVelocity = VelocitySize * ReduceSwingForceVelocity;

	const float Input = MovementInput * FinalVelocity;
	
	const FVector PlayerRightVector = GetActorRightVector();
	
	const FVector FinalPlayerRightVector = FVector(PlayerRightVector.X, PlayerRightVector.Y, 0.f).GetSafeNormal();

	const FVector FinalForce = UKismetMathLibrary::Multiply_VectorFloat(FinalPlayerRightVector, Input);

	return FinalForce;
}

FVector APlayerManaCharacter::GamepadForwardSwingForce(const float MovementInput) const
{
	float VelocitySize = GetVelocity().Size();
	VelocitySize = UKismetMathLibrary::FClamp(VelocitySize, 0.f, 1000.f);

	const float ReduceSwingForceVelocity = 50.f / SwingSpeedBalancer;

	const float FinalVelocity = VelocitySize * ReduceSwingForceVelocity;

	const float Input = MovementInput * FinalVelocity;

	const FVector PlayerForwardVector = GetActorForwardVector();

	const FVector FinalPlayerForwardVector = FVector(PlayerForwardVector.X, PlayerForwardVector.Y, 0.f).GetSafeNormal();

	const FVector FinalForce = UKismetMathLibrary::Multiply_VectorFloat(FinalPlayerForwardVector, Input);

	return FinalForce;
}

//////////////////////////////////////////////////////////////////////////
// Combat

void APlayerManaCharacter::Blocking()
{
	Super::Blocking();

	CurrentBlockingMontage = ShieldBlockMontage; //In the future, set this based on cur equipment


	bool bHasBlockingTag = GetAbilitySystemComponent()->HasMatchingGameplayTag(
		FGameplayTag::RequestGameplayTag(FName("Player.IsBlocking"))
	);

	if (!ManaPlayerAnimInstance) return;

	if (bHasBlockingTag)
	{
		ManaPlayerAnimInstance->SetIsBlocking(true);

		if (!ManaPlayerAnimInstance->Montage_IsPlaying(CurrentBlockingMontage))
		{
			ManaPlayerAnimInstance->SetIsBlocking(true);


			float Duration = ManaPlayerAnimInstance->Montage_Play(CurrentBlockingMontage, 1.0f);
			UE_LOG(LogTemp, Log, TEXT("Block montage play returned %f"), Duration);
		}
	}
	else
	{
		ManaPlayerAnimInstance->SetIsBlocking(false);

		if (ManaPlayerAnimInstance->Montage_IsPlaying(CurrentBlockingMontage))
		{
			ManaPlayerAnimInstance->Montage_Stop(0.05f, CurrentBlockingMontage);
		}
	}
		/*
	if (ManaPlayerAnimInstance)
	{
		if (!ManaPlayerAnimInstance->Montage_IsPlaying(CurrentAttackMontage)) {
			//AnimInstance->Montage_Play(CurrentBlockingMontage);
			ManaPlayerAnimInstance->SetIsBlocking(true);
			PlayAnimMontage(CurrentBlockingMontage);
		}
	}
	*/
}

void APlayerManaCharacter::FinishedBlocking()
{
	Super::FinishedBlocking();

	RemoveBlockEffect();

	if (ManaPlayerAnimInstance)
	{
		ManaPlayerAnimInstance->SetIsBlocking(false);
		ManaPlayerAnimInstance->Montage_Stop(.2f, CurrentBlockingMontage);
	}
}

void APlayerManaCharacter::MeleeAttackNotify(FVector AttackPosition, bool IsFinisher)
{
	Super::MeleeAttackNotify(AttackPosition, IsFinisher);
}

void APlayerManaCharacter::UpdateBlockingState() const
{
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Character.IsBlocking")))
	{
		if (!ManaPlayerAnimInstance->Montage_IsPlaying(CurrentBlockingMontage))
		{
			float Duration = ManaPlayerAnimInstance->Montage_Play(CurrentBlockingMontage, 1.0f);
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

bool APlayerManaCharacter::GetWon_Implementation() const
{
	if (const AManaPlayerController* ManaController = Cast<AManaPlayerController>(GetController()))
	{
		return ManaController->bDidIWin;
	}
	return false;
}

void APlayerManaCharacter::SetDefaultCombos()
{
	Super::SetDefaultCombos();

	NextAttackMontageSection = "Attack01";
	//GEngine->AddOnScreenDebugMessage(5, 5.f, FColor::Purple, "Combos Reset to Default.");

}

void APlayerManaCharacter::SetNextComboSegment(const FName NextCombo)
{
	Super::SetNextComboSegment(NextCombo);

	if (ActiveAttackAbility)
	{
		ActiveAttackAbility->EndAbilityAndListenForCombo();
		NextAttackMontageSection = NextCombo;
		//GEngine->AddOnScreenDebugMessage(101, 5.f, FColor::Purple, "Setting Next Combo Segment NOW!");
	}
}


//////////////////// -- Camera Stuff -- \\\\\\\\\\\\\\\\\\\\\\\

AManaCameraModificationVolume* APlayerManaCharacter::GetCurrentCameraModificationVolume() const
{
	return CurrentCameraModificationVolume;
}

void APlayerManaCharacter::SetCurrentCameraModificationVolume(AManaCameraModificationVolume* InCurrentCameraModificationVolume)
{
	CurrentCameraModificationVolume = InCurrentCameraModificationVolume;
}

bool APlayerManaCharacter::GotMovementInput() const
{
	return (!CachedInputDirection.IsNearlyZero());
}

bool APlayerManaCharacter::SelectTarget(bool SelectTarget)
{
	bShouldAddCameraTarget = SelectTarget;
	return bShouldAddCameraTarget;
}

AActor* APlayerManaCharacter::SetCurrentTarget(AActor* CurrentTarget)
{
	CurrentCamTarget = CurrentTarget;
	return CurrentTarget;
}

AActor* APlayerManaCharacter::GetCurrentTarget() const
{
	return CurrentCamTarget;
}

bool APlayerManaCharacter::IsSelectingTarget() const
{
	return bShouldAddCameraTarget;
}

void APlayerManaCharacter::OnBlockingTagChanged(const FGameplayTag Tag, int32 NewCount)
{
    // Tag was removed if NewCount == 0
    if (NewCount == 0)
    {
        FinishedBlocking();
    }
}

void APlayerManaCharacter::UpdateFocusedCamera(float DeltaTime)
{
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsFocused"))))
	{
		if (!ManaPlayerAnimInstance) return;

		if (CombatCameraTarget)
		{
			const float VerticalDist = FMath::Abs(GetActorLocation().Z - CombatCameraTarget->GetActorLocation().Z);

			const float Dist = GetDistanceTo(CombatCameraTarget);

			if (Dist < 600)
			{
				ManaPlayerAnimInstance->SetShouldTurnInPlace(true);
			}
			else
			{
				ManaPlayerAnimInstance->SetShouldTurnInPlace(false);
			}
			
			FRotator DesiredFocusRot;
		
			if (VerticalDist < 500)
			{
				DesiredFocusRot = GetCurrentFocusingDirection();
			}
			else
			{
				DesiredFocusRot = GetCurrentFocusingDirection();
			}

			const FRotator CurrentFocusRot = FMath::RInterpTo(Controller->GetControlRotation(), DesiredFocusRot, DeltaTime, .2f);
		
			Controller->SetControlRotation(CurrentFocusRot);
		
			if (!ManaPlayerAnimInstance->Montage_IsPlaying(GetRollMontage()))
			{
				SetActorRotation(FRotator(0, CurrentFocusRot.Yaw, 0));
			}
		}
	}
}

UCameraShakeBase* APlayerManaCharacter::ActivateCamShake(const TSubclassOf<UCameraShakeBase> ShakeType) const
{
	if (bCanCameraShake)
	{
		if (const AManaPlayerController* ManaController = Cast<AManaPlayerController>(GetController()))
		{
			if (ShakeType)
			{
				return ManaController->PlayerCameraManager->StartCameraShake(ShakeType);
			}
		}
	}
	return nullptr;
}

void APlayerManaCharacter::EndCameraShake(UCameraShakeBase* Shake) const
{
	if (Shake)
	{
		if (const AManaPlayerController* ManaController = Cast<AManaPlayerController>(GetController()))
		{
			ManaController->PlayerCameraManager->StopCameraShake(Shake);
		}
	}
}

void APlayerManaCharacter::RemoveBeaconFromList(ASkyBeacon* BeaconToRemove)
{
	Beacons.Remove(BeaconToRemove);

	if (Beacons.Num() == 0)
	{
		if (AManaPlayerController* ManaController = Cast<AManaPlayerController>(GetController()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Win called on player");
			ManaController->bDidIWin = true;
			ManaController->GetFadeWidget()->StartFadeToBlack();
		}
	}
}

FRotator APlayerManaCharacter::GetCurrentFocusingDirection() const
{
	if (CombatCameraTarget)
	{
		return UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), CombatCameraTarget->GetActorLocation());
	}
	return GetActorForwardVector().Rotation();
}

FVector APlayerManaCharacter::GetDirectionVectorToCombatTarget() const
{
	if (CombatCameraTarget)
	{
		return (CombatCameraTarget->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	}
	return GetActorForwardVector();
}


//////////////////// -- Input -- \\\\\\\\\\\\\\\\\\\\\\\

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
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &APlayerManaCharacter::AttackInput);

		// Rolling
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this, &APlayerManaCharacter::Roll);

		// Blocking/Focusing
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Triggered, this, &APlayerManaCharacter::Block);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Ongoing, this, &APlayerManaCharacter::Block);
		EnhancedInputComponent->BindAction(BlockAction, ETriggerEvent::Completed, this, &APlayerManaCharacter::StopBlock);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerManaCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Canceled, this, &APlayerManaCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &APlayerManaCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerManaCharacter::Look);

		// Hooking
		EnhancedInputComponent->BindAction(HookAction, ETriggerEvent::Triggered, this, &APlayerManaCharacter::Hook);

		// Hooking
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &APlayerManaCharacter::Equip);

		//Pausing
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &APlayerManaCharacter::Pause);

		//Dropping
		EnhancedInputComponent->BindAction(DropRightAction, ETriggerEvent::Started, this, &APlayerManaCharacter::DropRightInput);
		EnhancedInputComponent->BindAction(DropLeftAction, ETriggerEvent::Started, this, &APlayerManaCharacter::DropLeftInput);

		/* DEBUGGING */
		//Reload level
		EnhancedInputComponent->BindAction(DEBUG_ReloadAction, ETriggerEvent::Completed, this, &APlayerManaCharacter::DEBUG_ReloadLevel);
		
		//Refill Mana
		EnhancedInputComponent->BindAction(DEBUG_RefillManaAction, ETriggerEvent::Completed, this, &APlayerManaCharacter::DEBUG_RefillMana);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APlayerManaCharacter::Jump()
{
	SetJumpTimer(0.3f);
}

void APlayerManaCharacter::SetJumpTimer(float Time)
{
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning"))))
	{
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(WallJumpTagContainer, true);
		return;
	}

	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsSwinging"))))
	{
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(SwingJumpTagContainer, true);
		return;
	}
	
	CurrentJumpTimer = Time;
}

void APlayerManaCharacter::CountDownJumpTimer(float DeltaTime)
{
	CurrentJumpTimer -= DeltaTime;

	if (CurrentJumpTimer > 0 && !GetCharacterMovement()->IsFalling())
	{
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(JumpTagContainer, true);
		CurrentJumpTimer = 0.f;
	}
}

void APlayerManaCharacter::OnMantleEnded()
{
	GetMantleAbility()->OnMantleEnded();
}

void APlayerManaCharacter::Move(const FInputActionValue& Value)
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();
	bool IsFree = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.IsFree")));
	bool IsSwinging = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsSwinging")));
	bool IsFocusing = AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsFocused")));
	
	if (AbilitySystem)
	{
		// input is a Vector2D
		FVector2D MovementVector = Value.Get<FVector2D>();

		// Add or remove the running tag based on input
		FGameplayTag RunningTag = FGameplayTag::RequestGameplayTag(FName("Character.IsRunning"));
		if (!MovementVector.IsNearlyZero())
		{
			if (!AbilitySystem->HasMatchingGameplayTag(RunningTag))
			{
				AbilitySystem->AddLooseGameplayTag(RunningTag);
			}
			if (ManaPlayerAnimInstance)
			{
				ManaPlayerAnimInstance->SetIsRunning(true);
			}
		}
		else
		{
			if (AbilitySystem->HasMatchingGameplayTag(RunningTag))
			{
				AbilitySystem->RemoveLooseGameplayTag(RunningTag);
			}
			if (ManaPlayerAnimInstance)
			{
				ManaPlayerAnimInstance->SetIsRunning(false);
			}
		}

		if (Controller != nullptr)
		{
			// get forward vector
			FVector ForwardDirection;
			FVector RightDirection;
			if (IsFocusing)
			{
				// find out which way is forward
				const FRotator Rotation = GetCurrentFocusingDirection();
				const FRotator YawRotation(0, Rotation.Yaw, 0);
				ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			}
			else
			{
				// find out which way is forward
				const FRotator Rotation = Controller->GetControlRotation();
				const FRotator YawRotation(0, Rotation.Yaw, 0);
				ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
				RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
			}

			CachedInputDirection = (ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X).GetSafeNormal();

			if (IsFree)
			{
				// add movement 
				AddMovementInput(ForwardDirection, MovementVector.Y);
				AddMovementInput(RightDirection, MovementVector.X);
			}

			if (IsSwinging)
			{
				GetCharacterMovement()->AddForce(GamepadRightSwingForce(MovementVector.X));
				GetCharacterMovement()->AddForce(GamepadForwardSwingForce(MovementVector.Y));
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
		const float DeltaSeconds = GetWorld()->GetDeltaSeconds() * CamRotSpeed;
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X * DeltaSeconds);
		AddControllerPitchInput(LookAxisVector.Y * DeltaSeconds);
	}
}

bool APlayerManaCharacter::Attack()
{
	if (Super::Attack())
	{
		if (ManaPlayerAnimInstance)
		{
			ManaPlayerAnimInstance->SetIsAttacking(true);
		}
		return true;
	}
	return false;
}

void APlayerManaCharacter::AttackInput(const FInputActionValue& Value)
{
	Attack();
}

FGameplayTagContainer APlayerManaCharacter::GetAttackType() const
{
	FGameplayTagContainer FAttackType;
	if (const FGameplayTag AirTag = FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne")); GetAbilitySystemComponent()->HasMatchingGameplayTag(AirTag))
	{
		FAttackType = AirAttackTagContainer;
	}
	else
	{
		FAttackType = AttackTagContainer;
	}

	return FAttackType;
}

void APlayerManaCharacter::GetMontageToPlay()
{
	UAnimMontage* MontageToPlay;

	if (EquipmentState == EEquipmentState::EES_Unequipped)
	{
		if (GetCharacterMovement()->Velocity.IsNearlyZero())
		{
			//GEngine->AddOnScreenDebugMessage(-1,5.f, FColor::Purple, "Getting Equip Attack!");
			MontageToPlay = RightHandEquipment->GetEquipAttack();
			RemoveFreeTag();
		}
		else
		{
			MontageToPlay = RightHandEquipment->GetEquipAttackMovement();
		}
	}
	else
	{
		if (GetCharacterMovement()->Velocity.IsNearlyZero())
		{
			//If we're not moving
			RemoveFreeTag();
			MontageToPlay = RightHandEquipment->GetAttackCombo();
		}
		else
		{
			//If we're moving
			MontageToPlay = RightHandEquipment->GetAttackComboMovement();
		}
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Montage set to: %s"), *MontageToPlay->GetName()));
	if (MontageToPlay) SetAttackMontage(MontageToPlay);	
}

void APlayerManaCharacter::Block(const FInputActionValue& Value) 
{
	//if (GEngine && GetCharacterMovement()->IsFalling() == false) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Block");
	//}
	if (LeftHandEquipment)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "Left Hand Equipment found!");
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(BlockTagContainer, true);
	}

	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(FocusTagContainer, true);
}

void APlayerManaCharacter::RemoveBlockEffect() const
{
	FGameplayTagContainer Tag;
	Tag.AddTag(FGameplayTag::RequestGameplayTag(FName("Player.IsBlocking")));
	GetAbilitySystemComponent()->RemoveActiveEffectsWithGrantedTags(Tag);
}

void APlayerManaCharacter::StopBlock(const FInputActionValue& Value)
{
	//if (GEngine && GetCharacterMovement()->IsFalling() == false) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "StopBlock");
	//}

	RemoveBlockEffect();
	
	if (ActiveFocusAbility) ActiveFocusAbility->EndFocusAbility();
	
	UpdateStaminaRegen();
}

void APlayerManaCharacter::Roll(const FInputActionValue& Value) 
{
	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(RollTagContainer, true);
}

void APlayerManaCharacter::Hook(const FInputActionValue& Value)
{
	UAC_HookShot* HookShot = GetHookShot();
	if (HookShot->GetCurrentTarget())
	{
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(HookTagContainer, true);
		PlayFlashEffect(FVector(0.f, 0.f, 1.f), .5f);
		ManaPlayerAnimInstance->Montage_Play(GetThrowHookMontage());
	}
}

void APlayerManaCharacter::Equip(const FInputActionValue& Value)
{
	if (!LeftHandEquipment || !RightHandEquipment)
	{
		if (const FGameplayTag EquipTag = FGameplayTag::RequestGameplayTag("Character.IsEquipping"); !GetAbilitySystemComponent()->HasMatchingGameplayTag(EquipTag))
		{
			GrabOverlappingItem();
		}
	}
	
	if (!OverlappingItem)
	{
		if (RightHandEquipment || LeftHandEquipment)
		{
			if (GetAbilitySystemComponent()->TryActivateAbilitiesByTag(EquipTagContainer, true))
			{
				if (ManaPlayerAnimInstance)
				{
					ManaPlayerAnimInstance->SetIsEquipping(true);

				}
			}
		}
	}
}

void APlayerManaCharacter::Pause(const FInputActionValue& Value)
{
	if (UGameplayStatics::IsGamePaused(GetWorld()))
	{
		if (AManaPlayerController* ManaController = Cast<AManaPlayerController>(GetController()))
		{
			UGameplayStatics::SetGamePaused(GetWorld(), false);
			ManaController->HidePauseMenu();
		}
	}
	else
	{
		if (AManaPlayerController* ManaController = Cast<AManaPlayerController>(GetController()))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Pause called from player");
			UGameplayStatics::SetGamePaused(GetWorld(), true);
			ManaController->ShowPauseMenu();
		}
	}
}

void APlayerManaCharacter::DropRightInput(const FInputActionValue& Value)
{
	if (GetRightHandEquipment())
	{
		if (GetAbilitySystemComponent())
		{
			const FName& EquipSocket = GetRightHandEquipment()->GetEquipmentSocket(); 
			EquipGearToSocket(GetRightHandEquipment(), EquipSocket);
			
			CurrentDropMontage = DropRightMontage;
			GetAbilitySystemComponent()->TryActivateAbilitiesByTag(DropTagContainer);
		}
	}
}

void APlayerManaCharacter::DropLeftInput(const FInputActionValue& Value)
{
	if (GetLeftHandEquipment())
	{
		if (GetAbilitySystemComponent())
		{
			const FName& EquipSocket = GetLeftHandEquipment()->GetEquipmentSocket(); 
			EquipGearToSocket(GetLeftHandEquipment(), EquipSocket);
			CurrentDropMontage = DropLeftMontage;
			GetAbilitySystemComponent()->TryActivateAbilitiesByTag(DropTagContainer);
		}
	}
}

void APlayerManaCharacter::DEBUG_ReloadLevel(const FInputActionValue& Value)
{
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
}

void APlayerManaCharacter::DEBUG_RefillMana(const FInputActionValue& Value)
{
	if (AManaPlayerController* ManaController = Cast<AManaPlayerController>(GetController()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Win called on player");
		ManaController->bDidIWin = true;
		ManaController->GetFadeWidget()->StartFadeToBlack();
	}
	
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();
	AbilitySystem->ApplyGameplayEffectToSelf(ManaDebugRefillEffectClass->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystem->MakeEffectContext());
}

void APlayerManaCharacter::Die(const FVector& HitLocation)
{
	Super::Die(HitLocation);

	GetWorld()->GetTimerManager().ClearTimer(FlashTimerHandle);

	if (AManaPlayerController* ManaController = Cast<AManaPlayerController>(GetController()))
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Purple, "Die called on player");
		ManaController->GetFadeWidget()->StartFadeToBlack();
	}

	PlayerCharacterDieMontage(HitLocation);
	
	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(DeathTagContainer, true);

}


//////////////////// -- Ability Regen -- \\\\\\\\\\\\\\\\\\\\\\\

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

	if (StaminaBarWidgetComponent && StaminaBarInstance)
	{
		ShowStaminaTimer = ShowStaminaTimerMax;
		if (!bAlreadyShowing)
		{
			bAlreadyShowing = true;
			StaminaBarInstance->PlayAnimation(StaminaBarInstance->GetStartAnimation());
		}
	}
	
	if (bIsAttacking || bIsRolling)
	{
		return;
	}
	
	if (bIsBlocking)
	{
		AbilitySystem->ApplyGameplayEffectToSelf(StaminaRegenBlockEffectClass->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystem->MakeEffectContext());
	}
	else
	{
		AbilitySystem->ApplyGameplayEffectToSelf(StaminaRegenEffectClass->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystem->MakeEffectContext());
	}
}

void APlayerManaCharacter::UpdateStaminaBar(float DeltaTime)
{
	//Stamina Bar
	if (StaminaBarInstance && ShowStaminaTimer > 0.0f)
	{
		StaminaBarInstance->UpdateProgressBar(DeltaTime, GetStaminaAsRatio_Implementation());

		ShowStaminaTimer -= DeltaTime;

		if (ShowStaminaTimer <= 0.0f)
		{
			StaminaBarInstance->PlayAnimation(StaminaBarInstance->GetEndAnimation());
			bAlreadyShowing = false;
		}
	}
	
}

void APlayerManaCharacter::UpdateManaRegen()
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();
	
	const FGameplayTag WallRunTag = FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning"));
	if (AbilitySystem->HasMatchingGameplayTag(WallRunTag)) return;	
	if (!AbilitySystem || !ManaRegenEffectClass)
	{ 
		return;
	}

	AbilitySystem->RemoveActiveGameplayEffectBySourceEffect(ManaRegenEffectClass, AbilitySystem);

	
	bool bCanRegen = GetMana_Implementation() < ManaRegenThreshold;
	if (bCanRegen)
	{
		AbilitySystem->ApplyGameplayEffectToSelf(ManaRegenEffectClass->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystem->MakeEffectContext());
	}
}

void APlayerManaCharacter::LowHealthFlashing()
{
	PlayFlashEffect(FVector(1,0,0), 1);

	if (!bAlreadyFlashing)
	{
		bAlreadyFlashing = true;
		GetWorld()->GetTimerManager().SetTimer(FlashTimerHandle, this, &APlayerManaCharacter::LowHealthFlashing, LowHealthFlashTimeInterval, true);
	}
}

// Overrides
void APlayerManaCharacter::LoadMe()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Load Me called on player!"));
}

void APlayerManaCharacter::UnloadMe()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Unload Me called on player!"));

}

