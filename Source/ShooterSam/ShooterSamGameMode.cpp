// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterSamGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "ShooterSamPlayerController.h"
#include "ShooterAI.h"

AShooterSamGameMode::AShooterSamGameMode()
{
	// stub
}

void AShooterSamGameMode::BeginPlay()
{
	Super::BeginPlay();

	// AI startup is handled by each controller's OnPossess().

	InitializeEnemiesRemaining();
}

void AShooterSamGameMode::InitializeEnemiesRemaining()
{
	TrackedEnemies.Empty();
	EnemiesRemaining = 0;

	if (EnemyClassToCount)
	{
		TArray<AActor*> EnemyActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyClassToCount, EnemyActors);
		for (AActor* Enemy : EnemyActors)
		{
			if (!Enemy)
			{
				continue;
			}

			TrackedEnemies.Add(Enemy);
		}

		// Common setup: EnemyClassToCount is a Character BP class that might also be used by the player.
		// If we count the player here, victory will never trigger because "1 enemy" will always remain.
		if (APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0))
		{
			const int32 Removed = TrackedEnemies.Remove(PlayerPawn);
			if (Removed > 0)
			{
				UE_LOG(LogTemp, Display, TEXT("InitializeEnemiesRemaining: excluded PlayerPawn (%s) from enemy count."), *GetNameSafe(PlayerPawn));
			}
		}

		EnemiesRemaining = TrackedEnemies.Num();

		UE_LOG(LogTemp, Display, TEXT("EnemiesRemaining initialized to %d (Class=%s)"),
			EnemiesRemaining,
			*GetNameSafe(EnemyClassToCount));
		return;
	}

	// Fallback: if you didn't set EnemyClassToCount, count Shooter AI controllers in the world.
	// (BP_ShooterAI derives from AShooterAI so it will be included.)
	{
		TArray<AActor*> AIControllers;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AShooterAI::StaticClass(), AIControllers);
		EnemiesRemaining = AIControllers.Num();

		UE_LOG(LogTemp, Warning, TEXT("EnemyClassToCount not set. EnemiesRemaining=%d (counting AShooterAI controllers). Set EnemyClassToCount in GameMode for accurate counting."),
			EnemiesRemaining);
	}

	if (EnemiesRemaining <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnemiesRemaining is 0. If you spawn enemies later, call InitializeEnemiesRemaining() after spawning, or call NotifyEnemyKilled() from each enemy death."));
	}
}

void AShooterSamGameMode::NotifyEnemyKilled(AActor* EnemyActor)
{
	if (!EnemyActor)
	{
		return;
	}

	if (EnemyClassToCount)
	{
		if (TrackedEnemies.Remove(EnemyActor) <= 0)
		{
			// Already removed / not tracked.
			return;
		}

		EnemiesRemaining = TrackedEnemies.Num();
	}
	else
	{
		if (EnemiesRemaining <= 0)
		{
			return;
		}

		EnemiesRemaining = FMath::Max(EnemiesRemaining - 1, 0);
	}

	UE_LOG(LogTemp, Display, TEXT("NotifyEnemyKilled: %s -> EnemiesRemaining=%d"), *GetNameSafe(EnemyActor), EnemiesRemaining);

	if (EnemiesRemaining == 0)
	{
		if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
		{
			if (AShooterSamPlayerController* ShooterPC = Cast<AShooterSamPlayerController>(PC))
			{
				ShooterPC->ShowVictoryScreen();
			}
		}
	}
}
