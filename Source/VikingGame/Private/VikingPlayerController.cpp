// Fill out your copyright notice in the Description page of Project Settings.


#include "VikingPlayerController.h"
#include "Blueprint/UserWidget.h"

void AVikingPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UUserWidget>(this, HUDOverlayClass);

	}
	HUDOverlay->AddToViewport();
	HUDOverlay->SetVisibility(ESlateVisibility::Visible);
}