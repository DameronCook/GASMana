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
	//Nothing so far! Hooray! Now I'm gonna feel guilty when I DO have to fill this

	Super::Tick(DeltaTime);

	UAbilitySystemComponent* AbilitySystem = GetAbilitySystemComponent();

	if (AbilitySystem && AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Player.IsAirborne"))))
	{
		WallRunCheck();
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

void APlayerManaCharacter::WallRunCheck()
{

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

