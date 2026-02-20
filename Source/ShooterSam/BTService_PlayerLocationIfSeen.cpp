// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_PlayerLocationIfSeen.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ShooterSamAIUtils.h"

UBTService_PlayerLocationIfSeen::UBTService_PlayerLocationIfSeen()
{
	NodeName = TEXT("Update Player Location If Seen");

	// Tick periodically (not every frame)
	bNotifyTick = true;
	Interval = 0.2f;
	RandomDeviation = 0.05f;
}

void UBTService_PlayerLocationIfSeen::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();
	if (!AIController || !BlackboardComp)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(AIController, 0);
	if (!PlayerPawn)
	{
		BlackboardComp->ClearValue(BlackboardKey.SelectedKeyName);
		return;
	}

	if (!ShooterSamAIUtils::IsPlayerPawnAlive(PlayerPawn))
	{
		BlackboardComp->ClearValue(BlackboardKey.SelectedKeyName);
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
		return;
	}

	if (AIController->LineOfSightTo(PlayerPawn))
	{
		BlackboardComp->SetValueAsVector(BlackboardKey.SelectedKeyName, PlayerPawn->GetActorLocation());
		AIController->SetFocus(PlayerPawn);
	}
	else
	{
		BlackboardComp->ClearValue(BlackboardKey.SelectedKeyName);
		AIController->ClearFocus(EAIFocusPriority::Gameplay);
	}
}

