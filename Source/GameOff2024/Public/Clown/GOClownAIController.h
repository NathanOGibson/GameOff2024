// Copyright Nah Studios

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GOClownAIController.generated.h"

/**
 *
 */

class AGOClownCharacter;

UCLASS()
class GAMEOFF2024_API AGOClownAIController : public AAIController
{
	GENERATED_BODY()

public:
	AGOClownAIController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Function to control patrolling
	void GetPatrolPoint();

	void MoveToPatrolPoint();


	FVector PatrolPoint;

	void MoveToLocationSmoothly(const FVector& TargetLocation);

	bool HasReachedPatrolPoint(float ReachThreshold);

	// Function to control chasing the player
	void ChasePlayer();

	// Function to control searching behavior
	void SearchForPlayer();

private:
	float RotationSpeed = 2.2f; // Speed of rotation interpolation
};
