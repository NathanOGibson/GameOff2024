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
class AGOPatrolPoint;
class UNavigationPath;

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

	/** Handle setting AI active/inactive */
	void SetInactive();
	void SetActive(FVector NewCharacterLocation, FRotator NewCharacterRotation);

	/** Patrol functions */
	void InitialiseGOPatrolPointReferences();
	FVector GetPatrolPoint();
	void MoveToPatrolPoint();
	void ResetPatrolSettings();

	/** Chase function */
	void ChasePlayer();

	/** Search functions */
	FVector GetSearchPoint();
	void MoveToSearchPoint();

	/** Retreat function */
	void MoveToRetreatPoint(FVector NewLocation);

	/** state check */
	bool HasReachedLocation(FVector NewLocation, float ReachThreshold);




private:
	/* Character current location and rotation variables **/
	FVector CharacterLocation;
	FRotator CharacterRotation;

	/** Spline variables */
	int32 CurrentSplinePatrolIndex = 0;
	TObjectPtr<USplineComponent> SplinePath;

	/** Patrol variables */
	TArray<AGOPatrolPoint*> GOStoredPatrolPoints;
	TArray<AGOPatrolPoint*> GOPatrolPoints;
	AGOPatrolPoint* CachedGOPatrolPoint;
	bool bHasCached = false;
	FVector FirstPathPoint;
	FVector PatrolPoint;
	float PatrolDistance = 1000.0f;
	float CachedDistance = 1000.0f;
	float MaxPatrolAngle = 30.0f;

	/** Search variable */
	FVector SearchPoint;

	/** Search variable */
	FVector RetreatPoint;

	/** Movement variable */
	float RotationSpeed = 4.f;

	/** Player variable */
	AActor* Player;

public:
	FORCEINLINE void SetPatrolDistance(float NewPatrolDistance) { PatrolDistance = NewPatrolDistance; }
	FORCEINLINE void SetCachedDistance(float NewCachedDistance) { CachedDistance = NewCachedDistance; }
};
