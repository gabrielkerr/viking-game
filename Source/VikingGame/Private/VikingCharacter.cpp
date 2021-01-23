// Fill out your copyright notice in the Description page of Project Settings.


#include "VikingCharacter.h"
#include "Camera/CameraComponent.h"
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


	//HealthComponent->OnHealthChanged.AddDynamic(this, &ASCharacter::OnHealthChanged);
	
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
	else 
	{
		// Set variable bAttacking
		bAttacking = true;

		// Use TimerHandle to delay resetting bAttacking to false.
		GetWorldTimerManager().SetTimer(TimerHandle_AttackDuration, this, &AVikingCharacter::EndAttack, 0.1, false, 1.0f);

		UE_LOG(LogTemp, Log, TEXT("ATTACK!"));
	}
}

void AVikingCharacter::EndAttack()
{
	bAttacking = false;
	GetWorldTimerManager().ClearTimer(TimerHandle_AttackDuration);
}
