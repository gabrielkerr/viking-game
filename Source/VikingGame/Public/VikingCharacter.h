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

	UPROPERTY(EditDefaultsOnly, Category = "Health")
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

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	EMovementState MovementState;

	FTimerHandle TimerHandle_AttackDuration;

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

	void SetCharacterMovementState(EMovementState State);
};
