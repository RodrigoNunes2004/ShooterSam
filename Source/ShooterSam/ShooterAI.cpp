// Fill out your copyright notice in the Description page of Project Settings.

#include "ShooterAI.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterSamAIUtils.h"
#include "BrainComponent.h"

namespace ShooterAIBlackboardKeys
{
	static const FName PlayerLocation(TEXT("PlayerLocation"));
	static const FName StartLocation(TEXT("StartLocation"));
	static const FName LastKnownPlayerLocation(TEXT("LastKnownPlayerLocation"));
}

static void InitShooterAIBlackboard(UBlackboardComponent* Blackboard, const APawn* PlayerPawn, const APawn* MyPawn)
{
	if (!Blackboard)
	{
		return;
	}

	// Only set keys that actually exist in the Blackboard asset.
	{
		const FBlackboard::FKey Key = Blackboard->GetKeyID(ShooterAIBlackboardKeys::PlayerLocation);
		if (Key != FBlackboard::InvalidKey && PlayerPawn)
		{
			Blackboard->SetValueAsVector(ShooterAIBlackboardKeys::PlayerLocation, PlayerPawn->GetActorLocation());
		}
	}

	{
		const FBlackboard::FKey Key = Blackboard->GetKeyID(ShooterAIBlackboardKeys::StartLocation);
		if (Key != FBlackboard::InvalidKey && MyPawn)
		{
			Blackboard->SetValueAsVector(ShooterAIBlackboardKeys::StartLocation, MyPawn->GetActorLocation());
		}
	}
}

AShooterAI::AShooterAI()
{
	PrimaryActorTick.bCanEverTick = true;

	// match built-in AI controllers behavior (see CombatAIController)
	bStartAILogicOnPossess = true;
	bAttachToPawn = true;
}

void AShooterAI::BeginPlay()
{
	Super::BeginPlay();

	// Cache the player for BT setup / fallback logic
	if (!PlayerPawn)
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}
}

void AShooterAI::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	MyPawn = InPawn;

	if (!PlayerPawn)
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}

	if (EnemyAIBehaviorTree)
	{
		// Avoid restarting logic if something already started it (eg. BP).
		if (!GetBrainComponent() || !GetBrainComponent()->IsRunning())
		{
			const bool bStarted = RunBehaviorTree(EnemyAIBehaviorTree);
			UE_LOG(LogTemp, Display, TEXT("%s RunBehaviorTree(OnPossess) => %s (Pawn=%s)"),
				*GetActorNameOrLabel(),
				bStarted ? TEXT("Started") : TEXT("Failed"),
				*GetNameSafe(InPawn));
		}

		if (UBlackboardComponent* BB = GetBlackboardComponent())
		{
			InitShooterAIBlackboard(BB, PlayerPawn, MyPawn);
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("%s has no EnemyAIBehaviorTree set (Pawn=%s)"),
			*GetActorNameOrLabel(),
			*GetNameSafe(InPawn));
	}
}

void AShooterAI::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Reacquire player pawn if it changed (respawn, etc.)
	if (!PlayerPawn)
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	}

	// If the player is dead, stop tracking them.
	if (!ShooterSamAIUtils::IsPlayerPawnAlive(PlayerPawn))
	{
		// Hard stop AI logic so BT can't keep issuing MoveTo/focus updates.
		if (UBrainComponent* Brain = GetBrainComponent())
		{
			if (Brain->IsRunning())
			{
				Brain->StopLogic(TEXT("Player unavailable"));
			}
		}

		// Clear target-related BB keys so BT can fall back to return/home behavior.
		if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
		{
			if (BlackboardComp->GetKeyID(ShooterAIBlackboardKeys::PlayerLocation) != FBlackboard::InvalidKey)
			{
				BlackboardComp->ClearValue(ShooterAIBlackboardKeys::PlayerLocation);
			}
			if (BlackboardComp->GetKeyID(ShooterAIBlackboardKeys::LastKnownPlayerLocation) != FBlackboard::InvalidKey)
			{
				BlackboardComp->ClearValue(ShooterAIBlackboardKeys::LastKnownPlayerLocation);
			}
		}

		CachedLastKnownPlayerLocation = FVector::ZeroVector;
		bHasCachedLastKnownPlayerLocation = false;

		ClearFocus(EAIFocusPriority::Gameplay);
		StopMovement();

		// Force future ticks to reacquire (in case of respawn).
		PlayerPawn = nullptr;
		return;
	}

	// If a Behavior Tree + Blackboard are running, keep keys updated to match your BT:
	// - "PlayerLocation" is set only when we can see the player
	// - "LastKnownPlayerLocation" is set when we lose sight (investigate)
	if (UBlackboardComponent* BlackboardComp = GetBlackboardComponent())
	{
		const bool bHasPlayerKey = BlackboardComp->GetKeyID(ShooterAIBlackboardKeys::PlayerLocation) != FBlackboard::InvalidKey;
		const bool bHasLastKnownKey = BlackboardComp->GetKeyID(ShooterAIBlackboardKeys::LastKnownPlayerLocation) != FBlackboard::InvalidKey;

		if (PlayerPawn)
		{
			if (LineOfSightTo(PlayerPawn))
			{
				CachedLastKnownPlayerLocation = PlayerPawn->GetActorLocation();
				bHasCachedLastKnownPlayerLocation = true;

				if (bHasPlayerKey)
				{
					BlackboardComp->SetValueAsVector(ShooterAIBlackboardKeys::PlayerLocation, CachedLastKnownPlayerLocation);
				}

				// Optional: clear investigation target once we reacquire
				if (bHasLastKnownKey)
				{
					BlackboardComp->ClearValue(ShooterAIBlackboardKeys::LastKnownPlayerLocation);
				}
			}
			else
			{
				// We can't see the player. Clear "PlayerLocation" so "Can See Player?" decorator fails,
				// and set "LastKnownPlayerLocation" so the Investigate branch can run.
				if (bHasPlayerKey)
				{
					BlackboardComp->ClearValue(ShooterAIBlackboardKeys::PlayerLocation);
				}

				if (bHasLastKnownKey && bHasCachedLastKnownPlayerLocation)
				{
					BlackboardComp->SetValueAsVector(ShooterAIBlackboardKeys::LastKnownPlayerLocation, CachedLastKnownPlayerLocation);
				}
			}
		}
	}

	// Fallback chase logic (useful while iterating before a BT/BB is fully wired).
	// If a Behavior Tree is running, let it drive movement.
	if (!EnemyAIBehaviorTree && PlayerPawn)
	{
		if (LineOfSightTo(PlayerPawn))
		{
			SetFocus(PlayerPawn);
			MoveToActor(PlayerPawn, 200.0f);
		}
		else
		{
			ClearFocus(EAIFocusPriority::Gameplay);
			StopMovement();
		}
	}
}

void AShooterAI::StartBehaviorTree(APawn* Player)
{
	// Compatibility shim: allow external callers to provide the player pawn,
	// but don't restart the behavior tree from here (OnPossess owns startup).
	if (Player)
	{
		PlayerPawn = Player;
	}

	MyPawn = GetPawn();

	if (UBlackboardComponent* BB = GetBlackboardComponent())
	{
		InitShooterAIBlackboard(BB, PlayerPawn, MyPawn);
	}
}
