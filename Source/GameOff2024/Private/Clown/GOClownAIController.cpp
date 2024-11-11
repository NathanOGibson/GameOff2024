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
	if (!PatrolPoint.IsZero())
	{
		FVector CurrentLocation = GetPawn()->GetActorLocation();

		// Get the vector pointing from the AI to the first path point
		FVector DirectionToFirstPathPoint = FirstPathPoint - CurrentLocation;
		DirectionToFirstPathPoint.Z = 0; // Ensure it's horizontal by zeroing out the Z axis

		// Calculate the rotation needed to face the first path point
		FRotator TargetRotation = DirectionToFirstPathPoint.Rotation();
		FRotator CurrentRotation = GetPawn()->GetActorRotation();

		// Calculate the shortest rotation path (interpolating to target rotation)
		FRotator SmoothedRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, GetWorld()->GetDeltaSeconds(), RotationSpeed);

		// Apply the smoothed rotation
		GetPawn()->SetActorRotation(SmoothedRotation);

		// Move towards the patrol point when aligned (use a tolerance value)
		float RotationDifference = FMath::Abs(FMath::FindDeltaAngleDegrees(CurrentRotation.Yaw, TargetRotation.Yaw));
		if (RotationDifference < 5.0f) // Only start moving when the rotation is aligned
		{
			MoveToLocation(PatrolPoint); // Move AI towards patrol point
		}
	}
}

void AGOClownAIController::MoveToLocationSmoothly(const FVector& TargetLocation)
{
	//// Early exit if no patrol point is set
	//if (PatrolPoint.IsZero())
	//{
	//	return;
	//}

	//FVector CurrentLocation = GetPawn()->GetActorLocation();
	//FRotator CurrentRotation = GetPawn()->GetActorRotation();

	//// Calculate the direction to the patrol point
	//FVector DirectionToTarget = TargetLocation - CurrentLocation;
	//DirectionToTarget.Z = 0; // Ignore Z (vertical) component

	//// Calculate target rotation
	//FRotator TargetRotation = DirectionToTarget.Rotation();

	//// Interpolate rotation smoothly with Slerp
	//FQuat CurrentQuat = FQuat(CurrentRotation);
	//FQuat TargetQuat = FQuat(TargetRotation);

	//// Perform slerp to smoothly rotate towards target rotation
	//FQuat SmoothedRotation = FQuat::Slerp(CurrentQuat, TargetQuat, RotationSpeed * GetWorld()->GetDeltaSeconds());

	//// Apply the smoothed rotation
	//GetPawn()->SetActorRotation(SmoothedRotation);

	//// Calculate the distance and yaw difference between current and target rotation
	//float YawDifference = FMath::Abs(CurrentRotation.Yaw - TargetRotation.Yaw);

	//// Ensure yaw difference is within 180 degrees for smooth turning
	//if (YawDifference > 180.0f)
	//{
	//	YawDifference = 360.0f - YawDifference;
	//}

	//// If sufficiently close to the target location and facing the target rotation, start moving
	//if (YawDifference < 5.0f)
	//{
	//	DrawDebugSphere(GetWorld(), PatrolPoint, 10.0f, 10, FColor::Blue, false, 10.0f);
	//	MoveToLocation(TargetLocation); // This triggers the AI to move towards the target location
	//}
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