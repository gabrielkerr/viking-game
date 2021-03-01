// Fill out your copyright notice in the Description page of Project Settings.


#include "MainVikingAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "VikingCharacter.h"

void UMainVikingAnimInstance::NativeInitializeAnimation()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();

		if (Pawn)
		{
			AVikingCharacter* MainCharacter = Cast<AVikingCharacter>(Pawn);

			if (MainCharacter)
			{
				MainCharacterRef = MainCharacter;
			}
		}
	}
}

void UMainVikingAnimInstance::UpdateAnimationProperties()
{
	if (Pawn == nullptr)
	{
		Pawn = TryGetPawnOwner();
	}

	if (Pawn)
	{
		// Get horizontal speed (X, Y plane)
		FVector PawnVelocity = Pawn->GetVelocity();
		FVector LateralVelocity = FVector(PawnVelocity.X, PawnVelocity.Y, 0.0f);
		MovementSpeed = LateralVelocity.Size();

		// Get jumping
		bIsInAir = Pawn->GetMovementComponent()->IsFalling();

		if (!MainCharacterRef)
		{
			AVikingCharacter* MainCharacter = Cast<AVikingCharacter>(Pawn);

			if (MainCharacter)
			{
				MainCharacterRef = MainCharacter;
			}
		}
	}
}
