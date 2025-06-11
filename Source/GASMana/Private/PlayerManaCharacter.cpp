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

	if (AbilitySystem && GroundedEffectClass)
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
}


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

	if (GEngine && GetCharacterMovement()->IsFalling() == false) {
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, "AttackSWING");
	}

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

	if (!AbilitySystem || !AbilitySystem->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("Character.IsFree"))))
	{
		//This checks if the player is free to move before the player actually runs any moving functions, this way if we're rolling attacking etc it won't move as well.
		return;
	}
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
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
	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(AttackTagContainer, true);
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
}

void APlayerManaCharacter::Roll(const FInputActionValue& Value)
{
	GetAbilitySystemComponent()->TryActivateAbilitiesByTag(RollTagContainer, true);
}

