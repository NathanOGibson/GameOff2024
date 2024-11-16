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

	/** Patrol functions */
	FVector GetPatrolPoint();
	void MoveToPatrolPoint();
	void AdjustPatrolSettings();
	void ResetPatrolSettings();
	bool HasReachedPatrolPoint(float ReachThreshold);

	/** Chase functions */
	void ChasePlayer();

	/** Search functions */
	FVector GetSearchPoint();
	void MoveToSearchPoint();
	bool HasReachedSearchPoint(float ReachThreshold);

private:
	/////////////////////////////////////////////////////////////

	void TempFunc();

	void InitialiseGOPatrolPointReferences();


	TArray<AGOPatrolPoint*> GOStoredPatrolPoints;
	TArray<AGOPatrolPoint*> GOPatrolPoints;

	AGOPatrolPoint* CachedGOPatrolPoint;
	bool bHasCached = false;

	/////////////////////////////////////////////////////////////


	/* Character current location and rotation variables **/
	FVector CharacterLocation;
	FRotator CharacterRotation;

	/** Spline variables */
	int32 CurrentSplinePatrolIndex = 0;
	TObjectPtr<USplineComponent> SplinePath;

	/** Patrol variables */
	FVector FirstPathPoint;
	FVector PatrolPoint;
	float PatrolDistance = 1000.0f;
	float CachedDistance = 1000.0f;
	float MaxPatrolAngle = 30.0f;

	/** Search variables */
	FVector SearchPoint;

	/** Movement variable */
	float RotationSpeed = 4.f;

	/** Player variable */
	AActor* Player;

public:
	FORCEINLINE void SetPatrolDistance(float NewPatrolDistance) { PatrolDistance = NewPatrolDistance; }
	FORCEINLINE void SetCachedDistance(float NewCachedDistance) { CachedDistance = NewCachedDistance; }
};
