// Copyright Epic Games, Inc. All Rights Reserved.

#include "ShooterSamGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "ShooterSamPlayerController.h"

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
	EnemiesRemaining = 0;

	if (!EnemyClassToCount)
	{
		return;
	}

	TArray<AActor*> EnemyActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), EnemyClassToCount, EnemyActors);
	EnemiesRemaining = EnemyActors.Num();
}

void AShooterSamGameMode::NotifyEnemyKilled(AActor* EnemyActor)
{
	if (EnemiesRemaining <= 0)
	{
		return;
	}

	EnemiesRemaining = FMath::Max(EnemiesRemaining - 1, 0);

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
