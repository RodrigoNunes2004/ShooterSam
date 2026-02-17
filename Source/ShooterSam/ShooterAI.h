// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "ShooterAI.generated.h"

/**
 * 
 */
class UBehaviorTree;

UCLASS()
class SHOOTERSAM_API AShooterAI : public AAIController
{
	GENERATED_BODY()

public:
	AShooterAI();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

public: 
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	UBehaviorTree* EnemyAIBehaviorTree;

	APawn* PlayerPawn = nullptr;
	APawn* MyPawn = nullptr;

	void StartBehaviorTree(APawn* Player);

private:
	FVector CachedLastKnownPlayerLocation = FVector::ZeroVector;
	bool bHasCachedLastKnownPlayerLocation = false;
};
