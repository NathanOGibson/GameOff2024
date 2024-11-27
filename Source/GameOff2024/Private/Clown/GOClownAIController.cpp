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
#include "BehaviorTree/BehaviorTreeComponent.h"
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

void AGOClownAIController::SetInactive()
{
	// Update position
	if (GetPawn())
	{
		GetPawn()->SetActorLocation(FVector::ZeroVector);
	}

	// Pause AI logic
	if (BrainComponent)
	{
		BrainComponent->PauseLogic(TEXT("AI set to inactive"));
	}

	// Stop any ongoing movement
	StopMovement();

	// Clear current AI tasks to ensure no lingering behavior
	if (UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		BTComponent->StopTree(EBTStopMode::Safe); // Safely stops the current behavior tree
	}

	// Resets patrol point logic
	GOPatrolPoints.Empty();
	ResetPatrolSettings();

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("AI set to inactive"));
}

void AGOClownAIController::SetActive(FVector NewCharacterLocation, FRotator NewCharacterRotation)
{
	// Update position and rotation
	if (GetPawn())
	{
		GetPawn()->SetActorLocation(NewCharacterLocation);
		GetPawn()->SetActorRotation(NewCharacterRotation);
	}

	// Reset AI tasks and resume logic
	if (UBehaviorTreeComponent* BTComponent = Cast<UBehaviorTreeComponent>(BrainComponent))
	{
		BTComponent->RestartTree(); // Restarts the behavior tree cleanly
	}

	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("AI set to active"));
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


FVector AGOClownAIController::GetPatrolPoint()
{
	// Return early if there are no patrol points
	if (GOPatrolPoints.Num() == 0)
	{
		return FVector::ZeroVector;
	}

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
		// Find a path to the cached patrol point
		BestNavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, CharacterLocation, CachedGOPatrolPoint->GetActorLocation());
		if (BestNavPath && BestNavPath->IsValid() && BestNavPath->PathPoints.Num() > 0)
		{
			SelectedPatrolPoint = CachedGOPatrolPoint;
			BestPatrolPoint = CachedGOPatrolPoint->GetActorLocation();
			bHasCached = true;
		}
	}

	// Loop through all preplaced patrol points
	for (AGOPatrolPoint* GOPatrolPoint : GOPatrolPoints)
	{
		FVector PatrolPointLocation = GOPatrolPoint->GetActorLocation();
		float DistanceToPatrolPoint = FVector::Dist(CharacterLocation, PatrolPointLocation);

		// Skip points outside the maximum patrol distance
		if (DistanceToPatrolPoint > PatrolDistance) continue;


		// Find a path to the patrol point using the navigation system
		UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, CharacterLocation, PatrolPointLocation);
		if (!NavPath || !NavPath->IsValid() || NavPath->PathPoints.Num() == 0) continue;


		// Update the best patrol point if this one is farther and no cached point exists
		if (DistanceToPatrolPoint > BestDistance && !bHasCached)
		{
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

			CachedGOPatrolPoint = GOPatrolPoint;
			CachedGOPatrolPoint->SetMaterialToCached();
			CachedThisLoop = true;
		}
	}

	// Handle the case where no valid path was found
	if (!BestNavPath) return FVector::ZeroVector;

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

void AGOClownAIController::MoveToSplinePoints()
{
	if (SplinePath->GetNumberOfSplinePoints() <= 1) return;

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


void AGOClownAIController::ResetPatrolSettings()
{
	PatrolPoint = FVector::ZeroVector;
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

void AGOClownAIController::ChasePlayer()
{
	if (!Player || !GetPawn()) return;

	MoveToSplinePoints();

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

	DrawDebugSphere(GetWorld(), CharacterLocation, 500.f, 8, FColor::Yellow, false, 100.f);

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, CharacterLocation, Player->GetActorLocation());
	if (!NavPath || !NavPath->IsValid() || NavPath->PathPoints.Num() == 0)
	{
		return FVector::ZeroVector;
	}
	else
	{
		SearchPoint = NavPath->PathPoints.Last();
	}

	SplinePath->ClearSplinePoints();
	for (const FVector& PathPoint : NavPath->PathPoints)
	{
		DrawDebugSphere(GetWorld(), PathPoint, 50.f, 8, FColor::Red, false, 100.f);
		SplinePath->AddSplinePoint(PathPoint, ESplineCoordinateSpace::World);
	}

	return SearchPoint;
}

void AGOClownAIController::MoveToRetreatPoint(FVector NewLocation)
{
	RetreatPoint = NewLocation;
	// Get the direction from the AI to the search point
	FVector DirectionToRetreatPoint = (NewLocation - CharacterLocation).GetSafeNormal();

	// Calculate and smoothly interpolate the AI's rotation to face the search point
	FRotator TargetRotation = DirectionToRetreatPoint.Rotation();
	FRotator SmoothedRotation = FMath::RInterpTo(CharacterRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);
	GetPawn()->SetActorRotation(SmoothedRotation);

	// Draw a debug sphere at the search point and move the AI toward it
	//DrawDebugSphere(GetWorld(), SearchPoint, 50.f, 8, FColor::Red, false, 0.f);
	MoveToLocation(NewLocation);
}

bool AGOClownAIController::HasReachedLocation(FVector NewLocation, float ReachThreshold)
{
	float DistanceToNewLocation = FVector::Dist(CharacterLocation, NewLocation);
	return DistanceToNewLocation <= ReachThreshold;
}
