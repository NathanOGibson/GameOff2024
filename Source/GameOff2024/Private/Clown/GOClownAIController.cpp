// Copyright Nah Studios


#include "Clown/GOClownAIController.h"
#include "Clown/GOClownCharacter.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Clown/Patrol/GOPatrolPoint.h"
AGOClownAIController::AGOClownAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	SplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("SplinePath"));
}

void AGOClownAIController::BeginPlay()
{
	Super::BeginPlay();
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	InitialiseGOPatrolPointReferences();
}

void AGOClownAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CharacterLocation = GetPawn()->GetActorLocation();
	CharacterRotation = GetPawn()->GetActorRotation();
}

void AGOClownAIController::InitialiseGOPatrolPointReferences()
{
	// Array to hold all actors of class AGOPatrolPoint found in the world
	TArray<AActor*> OutActors;

	// Get all actors of class AGOPatrolPoint in the current world and store them in OutActors
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGOPatrolPoint::StaticClass(), OutActors);

	for (AActor* Actor : OutActors)
	{
		AGOPatrolPoint* PatrolPointActor = Cast<AGOPatrolPoint>(Actor);
		if (PatrolPointActor) GOStoredPatrolPoints.Add(PatrolPointActor);

		PatrolPointActor->SetToMaterialActive();
	}

	GOPatrolPoints = GOStoredPatrolPoints;
}

void AGOClownAIController::TempFunc()
{
	/*
	// Calculate a random patrol direction based on the forward vector and a random angle
	FVector PatrolDirection = FRotator(0, FMath::RandRange(-MaxPatrolAngle, MaxPatrolAngle), 0).RotateVector(CharacterRotation.Vector());

	// Determine the target patrol point
	FVector TargetPatrolPoint = CharacterLocation + PatrolDirection * PatrolDistance;

	DrawDebugSphere(GetWorld(), TargetPatrolPoint, 8.f, 8.f, FColor::Red, false, 1000.f);

	// Find a path to the patrol point within the navigation mesh
	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, CharacterLocation, TargetPatrolPoint);
	if (!NavPath || !NavPath->IsValid() || NavPath->PathPoints.Num() == 0) return FVector::ZeroVector;

	SplinePath->ClearSplinePoints();

	for (const FVector& PathPoint : NavPath->PathPoints)
	{
		SplinePath->AddSplinePoint(PathPoint, ESplineCoordinateSpace::World);
		//DrawDebugSphere(GetWorld(), PathPoint, 8.f, 8.f, FColor::Green, false, 5.f);
	}

	// Set the final patrol point to the last valid point in the path
	PatrolPoint = NavPath->PathPoints.Last();
	FirstPathPoint = NavPath->PathPoints[1];

	return PatrolPoint;*/
}

FVector AGOClownAIController::GetPatrolPoint()
{
	// Define variables for patrol point selection
	AGOPatrolPoint* SelectedPatrolPoint = nullptr;
	FVector BestPatrolPoint = FVector::ZeroVector;
	float BestDistance = 0.f;
	CurrentSplinePatrolIndex = 0;
	bool CachedThisLoop = false;

	UNavigationPath* BestNavPath = nullptr;

	// Check if a cached patrol point exists
	if (CachedGOPatrolPoint != nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Using cached patrol point"));

		// Find a path to the cached patrol point
		BestNavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, CharacterLocation, CachedGOPatrolPoint->GetActorLocation());
		if (BestNavPath && BestNavPath->IsValid() && BestNavPath->PathPoints.Num() > 0)
		{
			SelectedPatrolPoint = CachedGOPatrolPoint;
			BestPatrolPoint = CachedGOPatrolPoint->GetActorLocation();
			bHasCached = true;
		}
	}

	// Return early if there are no patrol points
	if (GOPatrolPoints.Num() == 0)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("No patrol points available"));
		return FVector::ZeroVector;
	}

	// Loop through all preplaced patrol points
	for (AGOPatrolPoint* GOPatrolPoint : GOPatrolPoints)
	{
		FVector PatrolPointLocation = GOPatrolPoint->GetActorLocation();
		float DistanceToPatrolPoint = FVector::Dist(CharacterLocation, PatrolPointLocation);

		// Skip points outside the maximum patrol distance
		if (DistanceToPatrolPoint > PatrolDistance)
			continue;

		// Find a path to the patrol point using the navigation system
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, CharacterLocation, PatrolPointLocation);
		if (!NavPath || !NavPath->IsValid() || NavPath->PathPoints.Num() == 0)
			continue;

		// Update the best patrol point if this one is farther and no cached point exists
		if (DistanceToPatrolPoint > BestDistance && !bHasCached)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Selecting default patrol point"));

			BestDistance = DistanceToPatrolPoint;
			BestPatrolPoint = PatrolPointLocation;
			BestNavPath = NavPath;
			SelectedPatrolPoint = GOPatrolPoint;
		}

		// Cache a nearby patrol point if not already cached in this loop
		if (DistanceToPatrolPoint < CachedDistance && GOPatrolPoint != CachedGOPatrolPoint && !CachedThisLoop)
		{
			if (CachedGOPatrolPoint != nullptr)
			{
				float DistanceBetweenCachedPoints = FVector::Dist(PatrolPointLocation, CachedGOPatrolPoint->GetActorLocation());
				if (DistanceBetweenCachedPoints < CachedDistance) continue;
			}

			GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Blue, TEXT("Caching patrol point"));

			CachedGOPatrolPoint = GOPatrolPoint;
			CachedGOPatrolPoint->SetMaterialToCached();
			CachedThisLoop = true;
		}
	}

	// Handle the case where no valid path was found
	if (!BestNavPath)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("No valid path found"));
		return FVector::ZeroVector;
	}

	// Clear the cached patrol point if it matches the selected one
	if (SelectedPatrolPoint == CachedGOPatrolPoint)
	{
		CachedGOPatrolPoint = nullptr;
	}

	// Remove the selected patrol point from the list and update its material
	if (SelectedPatrolPoint)
	{
		GOPatrolPoints.Remove(SelectedPatrolPoint);
		SelectedPatrolPoint->SetMaterialToDisabled();
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, FString::Printf(TEXT("GOPatrolPoints: %d"), GOPatrolPoints.Num()));
	}

	// Clear and populate the spline path
	SplinePath->ClearSplinePoints();
	for (const FVector& PathPoint : BestNavPath->PathPoints)
	{
		//DrawDebugSphere(GetWorld(), PathPoint, 30.f, 8.f, FColor::Orange, false, 10.f);
		SplinePath->AddSplinePoint(PathPoint, ESplineCoordinateSpace::World);
	}

	// Set the final patrol point to the last valid point in the path
	PatrolPoint = BestNavPath->PathPoints.Last();

	// Optionally draw the chosen patrol point for debugging
	//DrawDebugSphere(GetWorld(), BestPatrolPoint, 30.f, 8.f, FColor::Blue, false, 10.f);

	return PatrolPoint;
}

void AGOClownAIController::MoveToPatrolPoint()
{
	if (PatrolPoint.IsZero() || SplinePath->GetNumberOfSplinePoints() <= 1) return;

	// Get the current patrol point from the spline
	FVector NextPathPoint = SplinePath->GetLocationAtSplinePoint(CurrentSplinePatrolIndex, ESplineCoordinateSpace::World);

	// Calculate direction and rotation toward the next patrol point
	FVector DirectionToNextPoint = (NextPathPoint - CharacterLocation).GetSafeNormal2D(); // Normalize and keep horizontal (2D)
	FRotator TargetRotation = DirectionToNextPoint.Rotation();

	// Smoothly interpolate to the target rotation and apply it
	FRotator SmoothedRotation = FMath::RInterpTo(CharacterRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);
	GetPawn()->SetActorRotation(SmoothedRotation);

	// Move the AI towards the next patrol point
	MoveToLocation(NextPathPoint);

	// Check if close enough to the next patrol point
	const float ReachedThreshold = 100.0f;
	if (FVector::DistSquared(CharacterLocation, NextPathPoint) < FMath::Square(ReachedThreshold))
	{
		// Increment patrol index and loop if necessary
		CurrentSplinePatrolIndex = (CurrentSplinePatrolIndex + 1) % SplinePath->GetNumberOfSplinePoints();
	}
}

void AGOClownAIController::AdjustPatrolSettings()
{
	/*MaxPatrolAngle += 10.f;
	if (PatrolDistance > 400.f) PatrolDistance -= 100.f;*/
}

void AGOClownAIController::ResetPatrolSettings()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Green, TEXT("Resetting"));
	bHasCached = false;
	CachedGOPatrolPoint = nullptr;
	if (GOPatrolPoints.Num() == 0)
	{
		for (AGOPatrolPoint* GOPatrolPoint : GOStoredPatrolPoints)
		{
			if (GOPatrolPoint) GOPatrolPoints.Add(GOPatrolPoint);
			GOPatrolPoint->SetToMaterialActive();
		}
	}
}

bool AGOClownAIController::HasReachedPatrolPoint(float ReachThreshold)
{
	float DistanceToPatrolPoint = FVector::Dist(CharacterLocation, PatrolPoint);

	// Check if the distance is within a defined threshold
	return DistanceToPatrolPoint <= ReachThreshold;
}

void AGOClownAIController::ChasePlayer()
{
	if (!Player || !GetPawn()) return;

	// Calculate direction to the player and the desired rotation
	const FVector DirectionToPlayer = (Player->GetActorLocation() - CharacterLocation).GetSafeNormal();
	const FRotator TargetRotation = DirectionToPlayer.Rotation();

	// Smoothly interpolate the AI's rotation toward the target rotation
	const FRotator SmoothedRotation = FMath::RInterpTo(CharacterRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);

	// Apply the smoothed rotation and move the AI toward the player's location
	GetPawn()->SetActorRotation(SmoothedRotation);
	MoveToLocation(Player->GetActorLocation());
}

FVector AGOClownAIController::GetSearchPoint()
{
	if (!Player) return FVector::ZeroVector;

	SearchPoint = Player->GetActorLocation();
	return SearchPoint;
}

void AGOClownAIController::MoveToSearchPoint()
{
	if (SearchPoint.IsZero()) return;

	// Get the direction from the AI to the search point
	FVector DirectionToSearchPoint = (SearchPoint - CharacterLocation).GetSafeNormal();

	// Calculate and smoothly interpolate the AI's rotation to face the search point
	FRotator TargetRotation = DirectionToSearchPoint.Rotation();
	FRotator SmoothedRotation = FMath::RInterpTo(CharacterRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);
	GetPawn()->SetActorRotation(SmoothedRotation);

	// Draw a debug sphere at the search point and move the AI toward it
	//DrawDebugSphere(GetWorld(), SearchPoint, 50.f, 8, FColor::Red, false, 0.f);
	MoveToLocation(SearchPoint);
}

bool AGOClownAIController::HasReachedSearchPoint(float ReachThreshold)
{
	float DistanceToSearchPoint = FVector::Dist(CharacterLocation, SearchPoint);
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, FString::Printf(TEXT("Distance to search point: %f"), DistanceToSearchPoint));
	// Check if the distance is within a defined threshold
	return DistanceToSearchPoint <= ReachThreshold;
}
