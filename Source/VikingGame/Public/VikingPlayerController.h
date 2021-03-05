// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "VikingPlayerController.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS()
class VIKINGGAME_API AVikingPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	/* Class of HUDOverlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<UUserWidget> HUDOverlayClass;

	/* Actual instance of HUDOverlay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	UUserWidget* HUDOverlay;


protected:
	virtual void BeginPlay() override;

};
