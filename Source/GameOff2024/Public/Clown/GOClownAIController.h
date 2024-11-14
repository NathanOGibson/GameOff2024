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

	void IncraseMaxPatrolAngle();

	void ResetMaxPatrolAngle() { MaxPatrolAngle = 30.0f;  PatrolDistance = CachedPatrolDistance; }


	bool HasReachedPatrolPoint(float ReachThreshold);
	bool HasReachedSearchPoint(float ReachThreshold);

	// Function to control chasing the player
	void ChasePlayer();

	// Function to control searching behavior
	FVector GetSearchPoint();

	void MoveToSearchPoint();

private:
	FVector SearchPoint;

	float RotationSpeed = 6.f; // Speed of rotation interpolation

	float MaxPatrolAngle = 30.0f;

	int32 CurrentPatrolIndex = 0;

	TObjectPtr<USplineComponent> SplinePath;

	float PatrolDistance = 1000.0f;
	float CachedPatrolDistance;

public:
	FORCEINLINE void SetPatrolDistance(float NewPatrolDistance) { PatrolDistance = NewPatrolDistance; CachedPatrolDistance = NewPatrolDistance; }
};
