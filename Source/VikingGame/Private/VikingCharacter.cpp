// Fill out your copyright notice in the Description page of Project Settings.


#include "VikingCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "HealthComponent.h"
#include "MeleeWeapon.h"

// Sets default values
AVikingCharacter::AVikingCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->bUsePawnControlRotation = true; // NOTE uncomment this if we want camera rotation to stay fixed
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	HealthComp = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComp"));

	WeaponSocketName = "WeaponSocket";
	bAttacking = false;
	bDied = false;

}

// Called when the game starts or when spawned
void AVikingCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Spawn Weapon
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = GetWorld()->SpawnActor<AMeleeWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	// Snap weapon to socket on mesh
	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponSocketName);
	}


	HealthComp->OnHealthChanged.AddDynamic(this, &AVikingCharacter::OnHealthChanged);
	
}

// Called every frame
void AVikingCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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
	// TODO Set jump. Make sure jump looks natural does(n't?) loop
}

void AVikingCharacter::MoveForward(float Value)
{
	if (!bAttacking)
	{
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AVikingCharacter::MoveRight(float Value)
{
	if (!bAttacking)
	{
		AddMovementInput(GetActorRightVector(), Value);
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
