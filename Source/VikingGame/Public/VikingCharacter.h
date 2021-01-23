// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "VikingCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UHealthComponent;
class AMeleeWeapon;

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	UHealthComponent* HealthComp;

	AMeleeWeapon* CurrentWeapon;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AMeleeWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName WeaponSocketName;

	UPROPERTY(BlueprintReadOnly, Category = "Animation")
	bool bAttacking;

	FTimerHandle TimerHandle_AttackDuration;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginAttack();

	void EndAttack();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
