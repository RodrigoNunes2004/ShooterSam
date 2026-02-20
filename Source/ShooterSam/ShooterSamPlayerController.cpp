// Copyright Epic Games, Inc. All Rights Reserved.


#include "ShooterSamPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "ShooterSam.h"
#include "Widgets/Input/SVirtualJoystick.h"

void AShooterSamPlayerController::ShowGameOverScreen()
{
	if (!GameOverWidgetClass)
	{
		return;
	}

	if (!GameOverWidget)
	{
		GameOverWidget = CreateWidget<UUserWidget>(this, GameOverWidgetClass);
	}

	if (!GameOverWidget)
	{
		return;
	}

	if (!GameOverWidget->IsInViewport())
	{
		GameOverWidget->AddToViewport(100);
	}

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(GameOverWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	SetPause(true);
}

void AShooterSamPlayerController::ShowVictoryScreen()
{
	if (!VictoryWidgetClass)
	{
		return;
	}

	if (!VictoryWidget)
	{
		VictoryWidget = CreateWidget<UUserWidget>(this, VictoryWidgetClass);
	}

	if (!VictoryWidget)
	{
		return;
	}

	if (!VictoryWidget->IsInViewport())
	{
		VictoryWidget->AddToViewport(100);
	}

	bShowMouseCursor = true;
	bEnableClickEvents = true;
	bEnableMouseOverEvents = true;
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);

	FInputModeUIOnly InputMode;
	InputMode.SetWidgetToFocus(VictoryWidget->TakeWidget());
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);

	SetPause(true);
}

void AShooterSamPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (SVirtualJoystick::ShouldDisplayTouchInterface() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogShooterSam, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}

	HUD_Widget = CreateWidget<UHUD_Widget>(this, HUD_WidgetClass);
	if (HUD_Widget)
	{
		HUD_Widget->AddToViewport();
	}
}

void AShooterSamPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}
