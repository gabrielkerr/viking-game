// Fill out your copyright notice in the Description page of Project Settings.


#include "VikingCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "HealthComponent.h"

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

}

// Called when the game starts or when spawned
void AVikingCharacter::BeginPlay()
{
	Super::BeginPlay();
	
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
}

void AVikingCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void AVikingCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
}