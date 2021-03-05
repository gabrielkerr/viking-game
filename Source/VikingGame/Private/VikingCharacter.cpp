// Fill out your copyright notice in the Description page of Project Settings.


#include "VikingCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "HealthComponent.h"
#include "MeleeWeapon.h"
#include "Shield.h"

// Sets default values
AVikingCharacter::AVikingCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true; 
	SpringArmComp->bEnableCameraLag = true;
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	// Don't apply rotation to character mesh, camera boom only
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement and rotation 
	GetCharacterMovement()->bOrientRotationToMovement = true; // Allow mesh to rotate according to movement input...
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 525.f;
	GetCharacterMovement()->AirControl = 0.2f; // Value < 1.0 allows limited movement control in the air

	WeaponSocketName = "WeaponSocket";
	ShieldSocketName = "ShieldSocket";
	bAttacking = false;
	bDied = false;
	bShiftKeyDown = false;

	MovementState = EMovementState::EMS_Normal;
	StaminaState = EStaminaState::ESS_Normal;

	MaxWalkSpeed = 650.f;
	MaxSprintSpeed = 950.f;

	MaxStamina = 200.f;
	Stamina = MaxStamina;
	MinimumStaminaThreshold = 40.f;
	StaminaDrainRate = 40.f;
	StaminaRecoveryRate = 40.f;
}

// Called when the game starts or when spawned
void AVikingCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Spawn Weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = GetWorld()->SpawnActor<AMeleeWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
	Shield = GetWorld()->SpawnActor<AShield>(ShieldClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	// Snap weapon to socket on mesh
	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
	}

	if (Shield)
	{
		Shield->SetOwner(this);
		Shield->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, ShieldSocketName);
	}

	HealthComp->OnHealthChanged.AddDynamic(this, &AVikingCharacter::OnHealthChanged);
	
}

// Called every frame
void AVikingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle stamina state
	switch (StaminaState)
	{
		case EStaminaState::ESS_Normal:
			if (bShiftKeyDown)
			{
				Stamina -= StaminaDrainRate * DeltaTime;

				// If stamina is less than min threshold, change state
				if (Stamina <= MinimumStaminaThreshold)
				{
					SetCharacterStaminaState(EStaminaState::ESS_MinimumThreshold);
				}
				// Otherwise, stamina is still high. Continue as normal.
				GetCharacterMovement()->MaxWalkSpeed = MaxSprintSpeed;
				SetCharacterMovementState(EMovementState::EMS_Sprinting);
			}
			else
			{
				// Recover stamina up to the max.
				Stamina += StaminaRecoveryRate * DeltaTime;

				if (Stamina > MaxStamina)
				{
					Stamina = MaxStamina;
				}

				GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
				SetCharacterMovementState(EMovementState::EMS_Normal);
			}
			break;
		case EStaminaState::ESS_MinimumThreshold:
			if (bShiftKeyDown)
			{
				Stamina -= StaminaDrainRate * DeltaTime;

				if (Stamina <= 0.f)
				{
					// If stamina is depleted, we are exhausted 
					SetCharacterStaminaState(EStaminaState::ESS_Exhausted);
					GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
					SetCharacterMovementState(EMovementState::EMS_Normal);
				}
				else
				{
					// Otherwise, stamina to burn. Continue as normal.
					GetCharacterMovement()->MaxWalkSpeed = MaxSprintSpeed;
					SetCharacterMovementState(EMovementState::EMS_Sprinting);
				}
			}
			else
			{
				// Recover stamina. 
				Stamina += StaminaRecoveryRate * DeltaTime;

				if (Stamina > MinimumStaminaThreshold)
				{
					// Set stamina to normal if above minimum threshold.
					SetCharacterStaminaState(EStaminaState::ESS_Normal);
				}

				GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
				SetCharacterMovementState(EMovementState::EMS_Normal);
			}
			break;
		case EStaminaState::ESS_Exhausted:
			if (bShiftKeyDown)
			{
				// Stamina does not recover.
				Stamina = 0.f;
				SetCharacterStaminaState(EStaminaState::ESS_Exhausted);
			}
			else
			{
				// Recover stamina. 
				Stamina += StaminaRecoveryRate * DeltaTime;

				if (Stamina > 0.f)
				{
					// Set stamina to normal if above 0. 
					SetCharacterStaminaState(EStaminaState::ESS_ExhaustedRecovering);
				}
			}
			GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
			SetCharacterMovementState(EMovementState::EMS_Normal);
			break;
		case EStaminaState::ESS_ExhaustedRecovering:
			if (bShiftKeyDown)
			{
				// Stamina doesn't change.
				GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
				SetCharacterMovementState(EMovementState::EMS_Normal);
				SetCharacterStaminaState(EStaminaState::ESS_ExhaustedRecovering);
			}
			else
			{
				// Recover stamina. 
				Stamina += StaminaRecoveryRate * DeltaTime;

				if (Stamina > MinimumStaminaThreshold)
				{
					// Set stamina to normal if above minimum threshold.
					SetCharacterStaminaState(EStaminaState::ESS_Normal);
				}

				GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeed;
				SetCharacterMovementState(EMovementState::EMS_Normal);
			}
			break;
		default:
			break;
	}

}

// Called to bind functionality to input
void AVikingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Called to bind functionality to input
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AVikingCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AVikingCharacter::MoveRight);

	PlayerInputComponent->BindAxis("LookAround", this, &ACharacter::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacter::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AVikingCharacter::BeginAttack);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// TODO update sprinting in AnimBP to not use sprint animation if not moving
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &AVikingCharacter::ShiftKeyPressed);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &AVikingCharacter::ShiftKeyReleased);
}

void AVikingCharacter::MoveForward(float Value)
{
	if ((!bAttacking) && (Controller != nullptr))
	{
		// Find which direction is forward based on character's local axes
		FRotator CharacterRotation = Controller->GetControlRotation();
		FRotator CharacterYaw(0.f, CharacterRotation.Yaw, 0.f);
		FVector MovementDirection = FRotationMatrix(CharacterYaw).GetUnitAxis(EAxis::X);
		AddMovementInput(MovementDirection, Value);
	}
}

void AVikingCharacter::MoveRight(float Value)
{
	if ((!bAttacking) && (Controller != nullptr))
	{
		FRotator CharacterRotation = Controller->GetControlRotation();
		FRotator CharacterYaw = FRotator(0.f, CharacterRotation.Yaw, 0.f);
		FVector MovementDirection = FRotationMatrix(CharacterYaw).GetUnitAxis(EAxis::Y);
		AddMovementInput(MovementDirection, Value);
	}
}

void AVikingCharacter::BeginAttack()
{
	if (bAttacking)
	{
		return;
	}
	else if (CurrentWeapon)
	{
		// Set variable bAttacking
		bAttacking = true;
		float AttackDuration = 0.0F;
		CurrentWeapon->BeginAttack(AttackDuration);

		// Use TimerHandle to delay resetting bAttacking to false.
		GetWorldTimerManager().SetTimer(TimerHandle_AttackDuration, this, &AVikingCharacter::EndAttack, 0.1, false, AttackDuration);

		UE_LOG(LogTemp, Log, TEXT("ATTACK!"));
	}
}

// TODO Ideally, would like to set up a notification when attack animation ends.
void AVikingCharacter::EndAttack()
{
	bAttacking = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_AttackDuration);
}

void AVikingCharacter::OnHealthChanged(UHealthComponent* ActorHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("Health changed!"));
	if (Health <= 0.0F && !bDied)
	{
		// TODO Set up death animation
		UE_LOG(LogTemp, Log, TEXT("DIED!"));
		bDied = true;

		GetMovementComponent()->StopMovementImmediately();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		// Disable any player input to the character
		DetachFromControllerPendingDestroy();

		// Destroy player object after 10 seconds
		SetLifeSpan(10.0F);
	}
}

void AVikingCharacter::ShiftKeyPressed()
{
	bShiftKeyDown = true;
}

void AVikingCharacter::ShiftKeyReleased()
{
	bShiftKeyDown = false;
}


void AVikingCharacter::SetCharacterMovementState(const EMovementState State)
{
	MovementState = State;
}

void AVikingCharacter::SetCharacterStaminaState(const EStaminaState State)
{
	StaminaState = State;
}
