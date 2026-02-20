// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "HUD_Widget.h"

#include "ShooterSamPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AShooterSamPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

public:
	UPROPERTY(EditAnywhere)
	TSubclassOf<UHUD_Widget> HUD_WidgetClass;

	UPROPERTY(VisibleAnywhere)
	UHUD_Widget* HUD_Widget;

	// UI screens
	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> GameOverWidgetClass;

	UPROPERTY(EditAnywhere, Category = "UI")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowGameOverScreen();

	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowVictoryScreen();

private:
	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> GameOverWidget;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> VictoryWidget;
};
