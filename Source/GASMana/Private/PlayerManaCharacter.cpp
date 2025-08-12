// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerManaCharacter.h"
#include "ManaPlayerAnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/ManaSpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Effect/GE_ManaPlayerGrounded.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Actors/BaseManaEnemy.h"
#include "Actors/ManaHookParent.h"
#include "Components/AC_HookShot.h"
#include "Components/AC_WallRun.h"
#include "Ability/GA_ManaPlayerAirAttack.h"
#include "Ability/GA_ManaPlayerAttack.h"

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

	EquipmentState = EEquipmentState::EES_Unequipped;

	RightHandEquipment = AddEquipment(FName("RightHandEquipSocket"), GetRightHandEquipment());
	LeftHandEquipment = AddEquipment(FName("LeftHandEquipSocket"), GetLeftHandEquipment());

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

	//if (GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(1, .1f, FColor::Red, "MovementModeChangedCalled");
	//}

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

	float ReduceSwingForceVelocity = 50.f / SwingSpeedBalancer;

	float FinalVelocity = VelocitySize * ReduceSwingForceVelocity;

	float Input = MovementInput * FinalVelocity;

	FVector FinalPlayerForwardVector;

	FVector PlayerForwardVector = GetActorForwardVector();

	FinalPlayerForwardVector = FVector(PlayerForwardVector.X, PlayerForwardVector.Y, 0.f).GetSafeNormal();

	FVector FinalForce = UKismetMathLibrary::Multiply_VectorFloat(FinalPlayerForwardVector, Input);

	return FinalForce;
}

//////////////////////////////////////////////////////////////////////////
// Combat

void APlayerManaCharacter::Blocking()
{
	Super::Blocking();

	if (UManaPlayerAnimInstance* AnimInstance = Cast<UManaPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance->SetIsBlocking(true);
		CurrentBlockingMontage = ShieldBlockMontage; //In the future, set this based on cur equipment
		if (!AnimInstance->Montage_IsPlaying(CurrentBlockingMontage)) {
			//do something here, maybe it's overwriting itself above???
			//AnimInstance->Montage_Play(CurrentBlockingMontage);
			PlayAnimMontage(CurrentBlockingMontage);
		}
	}
}

void APlayerManaCharacter::FinishedBlocking()
{
	Super::FinishedBlocking();

	if (UManaPlayerAnimInstance* AnimInstance = Cast<UManaPlayerAnimInstance>(GetMesh()->GetAnimInstance()))
	{
		AnimInstance->SetIsBlocking(false);
		AnimInstance->Montage_Stop(.2f, CurrentBlockingMontage);
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

		// Hooking
		EnhancedInputComponent->BindAction(HookAction, ETriggerEvent::Triggered, this, &APlayerManaCharacter::Hook);

		// Hooking
		EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Triggered, this, &APlayerManaCharacter::Equip);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void APlayerManaCharacter::Jump()
{
	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();

	if (AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsWallRunning"))))
	{
		AbilitySystem->TryActivateAbilitiesByTag(WallJumpTagContainer, true);
		return;
	}
	AbilitySystem->TryActivateAbilitiesByTag(JumpTagContainer, true);

	if (AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsSwinging"))))
	{
		AbilitySystem->TryActivateAbilitiesByTag(SwingJumpTagContainer, true);
		return;
	}

	//if (GEngine) {
	//	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, "Jump!");
	//}
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

			if (IsSwinging)
			{
				GetCharacterMovement()->AddForce(GamepadRightSwingForce(MovementVector.X));
				GetCharacterMovement()->AddForce(GamepadForwardSwingForce(MovementVector.Y));
				GEngine->AddOnScreenDebugMessage(6, .1f, FColor::Purple, "Applying Swinging Force and input!");
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
	GetMontageToPlay();

	if (const FGameplayTagContainer AttackType = GetAttackType(); GetAbilitySystemComponent()->TryActivateAbilitiesByTag(AttackType, true))
	{
		UManaPlayerAnimInstance* AnimInstance = Cast<UManaPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		AnimInstance->SetIsAttacking(true);
	}
}

FGameplayTagContainer APlayerManaCharacter::GetAttackType() const
{
	FGameplayTagContainer fAttackType;
	FGameplayTag AirTag = FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne"));
	if (GetAbilitySystemComponent()->HasMatchingGameplayTag(AirTag))
	{
		fAttackType = AirAttackTagContainer;
	}
	else
	{
		fAttackType = AttackTagContainer;
	}

	return fAttackType;
}

void APlayerManaCharacter::GetMontageToPlay()
{
	UAnimMontage* MontageToPlay;

	if (EquipmentState == EEquipmentState::EES_Unequipped)
	{
		//I WILL have a smarter way of getting the attack montages from the WEAPONS rather than just storing all of the montages on the player. I'm just trying to make one system work right now christ.
		//TODO: Get current attack montage from the weapon we have.
		//This maybe should get put somewhere else at some point, but the equip left montage DOES need to be called
		PlayAnimMontage(GetEquipLeftMontage());

		if (GetCharacterMovement()->Velocity.IsNearlyZero())
		{
			MontageToPlay = GetEquipAttackMontageNoMovement();
			RemoveFreeTag();
		}
		else
		{
			MontageToPlay = GetEquipAttackMontage();
		}
	}
	else
	{
		if (GetCharacterMovement()->Velocity.IsNearlyZero())
		{
			RemoveFreeTag();
			MontageToPlay = GetAttackMontageNoMovement();
		}
		else
		{
			MontageToPlay = GetAttackMontage();
		}
	}

	SetAttackMontage(MontageToPlay);
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

void APlayerManaCharacter::Hook(const FInputActionValue& Value)
{
	UAC_HookShot* HookShot = GetHookShot();
	if (HookShot->GetCurrentTarget())
	{
		GetAbilitySystemComponent()->TryActivateAbilitiesByTag(HookTagContainer, true);
		PlayFlashEffect(FVector(0.f, 0.f, 1.f), .5f);
	}
}

void APlayerManaCharacter::Equip(const FInputActionValue& Value)
{
	if (GetAbilitySystemComponent()->TryActivateAbilitiesByTag(EquipTagContainer, true))
	{
		UManaPlayerAnimInstance* AnimInstance = Cast<UManaPlayerAnimInstance>(GetMesh()->GetAnimInstance());
		AnimInstance->SetIsEquipping(true);
	}

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

void APlayerManaCharacter::RemoveFreeTag()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, "RemoveFreeTag called!");
	FGameplayTag FreeTag = FGameplayTag::RequestGameplayTag(FName("Character.IsFree"));
	GetAbilitySystemComponent()->RemoveLooseGameplayTag(FreeTag);
}
