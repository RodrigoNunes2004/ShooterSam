// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTaskNode_Shoot.h"

#include "ShooterAI.h"
#include "ShooterSamCharacter.h"

UBTTaskNode_Shoot::UBTTaskNode_Shoot()
{
	NodeName = TEXT("Shoot At Player");
}

EBTNodeResult::Type UBTTaskNode_Shoot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	EBTNodeResult::Type Result = EBTNodeResult::Failed;

	AShooterAI* OwnerController = Cast<AShooterAI>(OwnerComp.GetAIOwner());
	if (OwnerController)
	{
		// AI's possessed pawn (the AI character)
		AShooterSamCharacter* OwnerCharacter = Cast<AShooterSamCharacter>(OwnerController->GetPawn());
		// Player pawn tracked by the AI controller (if any)
		AShooterSamCharacter* PlayerCharacter = Cast<AShooterSamCharacter>(OwnerController->PlayerPawn);

		if (OwnerCharacter && PlayerCharacter && PlayerCharacter->IsAlive)
		{
			OwnerCharacter->Shoot();
			Result = EBTNodeResult::Succeeded;
		}
	}

	return Result;
}


