// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MeleeWeapon.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UCapsuleComponent;

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

	// TODO Deal damage
	// TODO On overlap
	UFUNCTION()
	void HandleOverlap(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	// TODO Attack
	// NOTE Weapon could be a spear for now until better animations come in

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
