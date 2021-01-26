// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeleeWeapon.generated.h"

class UStaticMeshComponent;
class UCapsuleComponent;
class USphereComponent;
class UParticleSystem;

UCLASS()
class VIKINGGAME_API AMeleeWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeleeWeapon();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// For skeletal mesh of weapon
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	// Root of the weapon to attach to character
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	USphereComponent* SphereComp;

	// Collision boundary for weapon
	UPROPERTY(EditDefaultsOnly, Category = "Components")
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	UParticleSystem* WeaponHitEffect;

	// On overlap
	UFUNCTION()
	void HandleOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void BeginAttack(float &AttackDuration);

	void EndAttack();

};
