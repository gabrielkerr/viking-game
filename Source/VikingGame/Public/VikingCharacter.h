// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VikingCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UHealthComponent;
class AMeleeWeapon;
class AShield;

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	EMS_Normal UMETA(DisplayName = "Normal"),
	EMS_Sprinting UMETA(DisplayName = "Sprinting"),

	EMS_MAX UMETA(DisplayName = "DefaultMax")
};

UENUM(BlueprintType)
enum class EStaminaState : uint8
{
	ESS_Normal UMETA(DisplayName = "Normal"),
	ESS_MinimumThreshold UMETA(DisplayName = "MinimumThreshold"),
	ESS_Exhausted UMETA(DisplayName = "Exhausted"),
	ESS_ExhaustedRecovering UMETA(DisplayName = "ExhaustedRecovering"),

	ESS_MAX UMETA(DisplayName = "DefaultMax")
};

UCLASS()
class VIKINGGAME_API AVikingCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AVikingCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	UHealthComponent* HealthComp;

	AMeleeWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AMeleeWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Shield")
	TSubclassOf<AShield> ShieldClass;

	AShield* Shield;

	UPROPERTY(VisibleDefaultsOnly, Category = "Shield")
	FName ShieldSocketName;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bAttacking;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bDied;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bShiftKeyDown;

	UPROPERTY(BlueprintReadOnly, Category = "Enums")
	EMovementState MovementState;

	UPROPERTY(BlueprintReadOnly, Category = "Enums")
	EStaminaState StaminaState;

	FTimerHandle TimerHandle_AttackDuration;

	float MaxWalkSpeed;

	float MaxSprintSpeed;

	// Maximum stamina for the player
	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	float MaxStamina;

	// Current stamina for the player
	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	float Stamina;

	// Current stamina for the player
	UPROPERTY(BlueprintReadOnly, Category = "Stamina")
	float MinimumStaminaThreshold;

	// Measured in units per second
	float StaminaDrainRate;

	// Measured in units per second
	float StaminaRecoveryRate;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginAttack();

	void EndAttack();

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* ActorHealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	void ShiftKeyPressed();

	void ShiftKeyReleased();

	void SetCharacterMovementState(const EMovementState State);

	void SetCharacterStaminaState(const EStaminaState State);
};
