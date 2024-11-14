// Copyright Nah Studios


#include "Clown/GOClownAIController.h"
#include "Clown/GOClownCharacter.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"

AGOClownAIController::AGOClownAIController()
{
	PrimaryActorTick.bCanEverTick = true;
	SplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("SplinePath"));
}

void AGOClownAIController::BeginPlay()
{
	Super::BeginPlay();
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void AGOClownAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CharacterLocation = GetPawn()->GetActorLocation();
	CharacterRotation = GetPawn()->GetActorRotation();
}

FVector AGOClownAIController::GetPatrolPoint()
{
	// Calculate a random patrol direction based on the forward vector and a random angle
	FVector PatrolDirection = FRotator(0, FMath::RandRange(-MaxPatrolAngle, MaxPatrolAngle), 0).RotateVector(CharacterRotation.Vector());

	// Determine the target patrol point
	FVector TargetPatrolPoint = CharacterLocation + PatrolDirection * PatrolDistance;


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
	MaxPatrolAngle += 10.f;
	if (PatrolDistance > 400.f) PatrolDistance -= 100.f;
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
