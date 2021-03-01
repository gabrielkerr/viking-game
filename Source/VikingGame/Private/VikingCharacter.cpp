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

	if (bShiftKeyDown)
	{
		GetCharacterMovement()->MaxWalkSpeed = 950.0;
		SetCharacterMovementState(EMovementState::EMS_Sprinting);
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 650.0;
		SetCharacterMovementState(EMovementState::EMS_Normal);
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


void AVikingCharacter::SetCharacterMovementState(EMovementState State)
{
	MovementState = State;
}
