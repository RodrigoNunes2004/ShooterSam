// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ShooterSamCharacter.h"

namespace ShooterSamAIUtils
{
	inline bool IsPlayerPawnAlive(const APawn* Pawn)
	{
		if (!Pawn)
		{
			return false;
		}

		// If the pawn is our player character class, use the explicit alive flag.
		if (const AShooterSamCharacter* ShooterChar = Cast<AShooterSamCharacter>(Pawn))
		{
			return ShooterChar->IsAlive;
		}

		// Generic fallback: if it's no longer possessed, treat as "dead/unavailable" for AI targeting.
		// This matches your current death behavior (DetachFromControllerPendingDestroy()).
		return Pawn->GetController() != nullptr;
	}
}

