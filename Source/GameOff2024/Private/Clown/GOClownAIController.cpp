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
	MaxPatrolAngle = 30.0f;
}

void AGOClownAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

FVector AGOClownAIController::GetPatrolPoint()
{
	FVector PointToReturn = FVector::ZeroVector;

	FVector CurrentLocation = GetPawn()->GetActorLocation(); // Get current location of the character
	FRotator CurrentRotation = GetPawn()->GetActorRotation(); // Get current rotation

	// Get the forward vector of the character (direction it's facing)
	FVector ForwardVector = CurrentRotation.Vector();

	// Random distance (e.g., 300 units) to search for a patrol point in front of the character
	float PatrolDistance = 700.0f;

	// Find a random offset within the specified angle
	float RandomAngle = FMath::RandRange(-MaxPatrolAngle, MaxPatrolAngle);
	FRotator RandomRotation = FRotator(0, RandomAngle, 0); // Random Yaw Rotation based on angle
	FVector PatrolDirection = RandomRotation.RotateVector(ForwardVector); // Rotate the forward vector by the random angle

	// Calculate the patrol point by applying the direction and distance
	PatrolPoint = CurrentLocation + PatrolDirection * PatrolDistance;

	// Ensure the patrol point is within the navigation mesh
	FNavLocation ValidPatrolPoint;
	UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

	if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, CurrentLocation, PatrolPoint))
	{
		if (NavPath->IsValid() && NavPath->PathPoints.Num() > 0)
		{
			SplinePath->ClearSplinePoints();
			for (const FVector& PointLoc : NavPath->PathPoints)
			{
				SplinePath->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
				DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8.f, FColor::Green, false, 5.f);
			}

			// Set first path point for smooth rotation
			FirstPathPoint = NavPath->PathPoints[0];

			if (NavPath->PathPoints.Num() > 0)
			{
				PatrolPoint = NavPath->PathPoints[NavPath->PathPoints.Num() - 1.f];
			}

			PointToReturn = PatrolPoint;
			FirstPathPoint = NavPath->PathPoints[1];
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Failed to find path to passive point!"));
			DrawDebugSphere(GetWorld(), PatrolPoint, 10.0f, 10, FColor::Red, false, 10.0f);
		}
	}

	return PointToReturn;
}

void AGOClownAIController::MoveToPatrolPoint()
{
	if (PatrolPoint.IsZero()) return;

	// Ensure there are multiple spline points available
	if (SplinePath->GetNumberOfSplinePoints() > 1)
	{
		FVector CurrentLocation = GetPawn()->GetActorLocation();

		// Get the current spline point based on the patrol index
		FVector NextPathPoint = SplinePath->GetLocationAtSplinePoint(CurrentPatrolIndex, ESplineCoordinateSpace::World);
		FVector DirectionToNextPoint = NextPathPoint - CurrentLocation;
		DirectionToNextPoint.Z = 0; // Keep rotation on the horizontal plane

		// Calculate the desired rotation toward the next path point
		FRotator TargetRotation = DirectionToNextPoint.Rotation();
		FRotator CurrentRotation = GetPawn()->GetActorRotation();

		// Smoothly interpolate between current and target rotation
		FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);
		GetPawn()->SetActorRotation(SmoothedRotation); // Apply the smoothed rotation

		MoveToLocation(NextPathPoint); // Move AI towards the next point in the path
		// Check if AI is close enough to face the next point and the rotation is almost complete
		if (FMath::Abs(FRotator::NormalizeAxis(CurrentRotation.Yaw - TargetRotation.Yaw)) < 5.0f)
		{

			// Manually control the rotation, move AI towards the next point
		}

		// If close enough to the current path point, move to the next point in the spline path
		if (FVector::Dist(CurrentLocation, NextPathPoint) < 100.0f) // Threshold for considering the point reached
		{
			CurrentPatrolIndex++;

			// If AI has reached the last point, reset or loop the index
			if (CurrentPatrolIndex >= SplinePath->GetNumberOfSplinePoints())
			{
				CurrentPatrolIndex = 0; // Or handle differently if you want to stop the patrol or loop
			}
		}
	}
}

void AGOClownAIController::IncraseMaxPatrolAngle()
{
	MaxPatrolAngle += 10.f;
}

bool AGOClownAIController::HasReachedPatrolPoint(float ReachThreshold)
{
	FVector CurrentLocation = GetPawn()->GetActorLocation();
	float DistanceToPatrolPoint = FVector::Dist(CurrentLocation, PatrolPoint);

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("Distance to patrol point: %f"), DistanceToPatrolPoint));

	// Check if the distance is within a defined threshold
	return DistanceToPatrolPoint <= ReachThreshold;
}

void AGOClownAIController::ChasePlayer()
{
	AActor* Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if (Player)
	{
		MoveToLocation(Player->GetActorLocation()); // This triggers the AI to move towards the target location
		//MoveToLocationSmoothly(Player->GetActorLocation());
	}
}

void AGOClownAIController::SearchForPlayer()
{
	// Simple search routine (e.g., move to last known player position, wait, and search)
}