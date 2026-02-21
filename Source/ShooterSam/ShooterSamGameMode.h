// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ShooterSamGameMode.generated.h"

class AActor;

/**
 *  Simple GameMode for a third person game
 */
UCLASS(abstract)
class AShooterSamGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	
	/** Constructor */
	AShooterSamGameMode();

protected:
	virtual void BeginPlay() override;

public:
	// Victory condition support (optional): count enemies and show Victory when all are dead.
	UPROPERTY(EditAnywhere, Category = "Victory")
	TSubclassOf<AActor> EnemyClassToCount;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Victory")
	int32 EnemiesRemaining = 0;

	// Call from BeginPlay (or Blueprint) to initialize the count.
	UFUNCTION(BlueprintCallable, Category = "Victory")
	void InitializeEnemiesRemaining();

	// Call this from your enemy Blueprint when it dies.
	UFUNCTION(BlueprintCallable, Category = "Victory")
	void NotifyEnemyKilled(AActor* EnemyActor);

private:
	// Track level enemies so we only count each once.
	UPROPERTY(Transient)
	TSet<TWeakObjectPtr<AActor>> TrackedEnemies;
};



