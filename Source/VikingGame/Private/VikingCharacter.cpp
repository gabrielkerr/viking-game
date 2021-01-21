// Fill out your copyright notice in the Description page of Project Settings.


#include "VikingCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

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

	PlayerInputComponent->BindAction("Attack", IE_Pressed, this, &AVikingCharacter::Attack);
	// TODO Set jump. Make sure jump looks natural does(n't?) loop
}

void AVikingCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void AVikingCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}

void AVikingCharacter::Attack()
{
	if (AttackAnim && !bAttacking)
	{
		// TODO Should probably do this all in animation blueprint
		// Add Attack node to ABP
		// Set variable bAttacking
		bAttacking = true;
		// Transition from walk and idle to Attack based on bAttacking 
		// Use TimerHandle to delay resetting bAttacking to false.
		bAttacking = false;

		UE_LOG(LogTemp, Log, TEXT("ATTACK!"));
		GetMesh()->SetAnimation(AttackAnim);
		GetMesh()->PlayAnimation(AttackAnim, false);

		// TODO Set timer delay

		// Return to animation blueprint for animations
		//GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No attack animation assigned!"));
	}
}
