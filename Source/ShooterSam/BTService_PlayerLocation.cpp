// Fill out your copyright notice in the Description page of Project Settings.

#include "BTService_PlayerLocation.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UBTService_PlayerLocation::UBTService_PlayerLocation()
{
	NodeName = TEXT("Get Player Location");

	// Tick periodically (not every frame)
	bNotifyTick = true;
	Interval = 0.2f;
	RandomDeviation = 0.05f;
}

void UBTService_PlayerLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UWorld* World = OwnerComp.GetWorld();
	if (!World)
	{
		return;
	}

	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(World, 0);
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if (PlayerPawn && Blackboard)
	{
		// Use the key selected in the service details panel (must be a Vector key)
		Blackboard->SetValueAsVector(BlackboardKey.SelectedKeyName, PlayerPawn->GetActorLocation());
	}
}

