// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeapon.h"
#include "VikingCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Particles/ParticleSystem.h"
#include "kismet/GameplayStatics.h"

#include "DrawDebugHelpers.h"

// Sets default values
AMeleeWeapon::AMeleeWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	RootComponent = SphereComp;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	CapsuleComp->SetupAttachment(RootComponent);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AMeleeWeapon::BeginPlay()
{
	Super::BeginPlay();

	AActor* MyOwner = GetOwner();

	if (MyOwner)
	{
		CapsuleComp->IgnoreActorWhenMoving(MyOwner, true);
	}

	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &AMeleeWeapon::HandleOverlap);
}

void AMeleeWeapon::HandleOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* MyOwner = GetOwner();

	// TODO Probably a better way to avoid colliding with owner by ignoring a specific collision channel...
	if (OtherActor != MyOwner)
	{
		UE_LOG(LogTemp, Log, TEXT("Overlapped with weapon!"));

		// Apply damage to actor
		AVikingCharacter* OtherCharacter = Cast<AVikingCharacter>(OtherActor);
		if (OtherCharacter)
		{
			UGameplayStatics::ApplyDamage(OtherActor, 20.0F, MyOwner->GetInstigatorController(), MyOwner, DamageType);
			if (WeaponHitEffect)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), WeaponHitEffect, MeshComp->GetComponentLocation(), MeshComp->GetComponentRotation());
			}
		}
	}
}

// Called every frame
void AMeleeWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// AttackDuration - How long the current attack should take. Pass back to caller for timers.
void AMeleeWeapon::BeginAttack(float &AttackDuration)
{
	AttackDuration = 1.0F;
}

void AMeleeWeapon::EndAttack()
{
}

