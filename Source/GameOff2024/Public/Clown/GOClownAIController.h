// Copyright Nah Studios

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GOClownAIController.generated.h"

/**
 *
 */

class AGOClownCharacter;
class USplineComponent;

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
	FVector GetPatrolPoint();

	void MoveToPatrolPoint();


	FVector FirstPathPoint;
	FVector PatrolPoint;

	void MoveToLocationSmoothly(const FVector& TargetLocation);

	void IncraseMaxPatrolAngle();

	void ResetMaxPatrolAngle() { MaxPatrolAngle = 30.0f; }

	bool HasReachedPatrolPoint(float ReachThreshold);

	// Function to control chasing the player
	void ChasePlayer();

	// Function to control searching behavior
	void SearchForPlayer();

private:
	float RotationSpeed = 4.f; // Speed of rotation interpolation

	float MaxPatrolAngle = 30.0f;

	TObjectPtr<USplineComponent> SplinePath;
};
