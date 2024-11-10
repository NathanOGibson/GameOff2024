// Copyright Nah Studios


#include "Clown/GOClownAIController.h"
#include "Clown/GOClownCharacter.h"
#include "NavigationSystem.h"
#include "NavigationData.h"
#include "Kismet/GameplayStatics.h"

AGOClownAIController::AGOClownAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGOClownAIController::BeginPlay()
{
	Super::BeginPlay();
}

void AGOClownAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Chase player logic here, or call Patrol if no player is detected
}

void AGOClownAIController::GetPatrolPoint()
{
	FVector CurrentLocation = GetPawn()->GetActorLocation(); // Get current location of the character
	FRotator CurrentRotation = GetPawn()->GetActorRotation(); // Get current rotation

	// Define the maximum angle (in degrees) the AI can move to for the patrol point
	float MaxPatrolAngle = 30.0f; // This can be adjusted based on how wide you want the patrol zone

	// Get the forward vector of the character (direction it's facing)
	FVector ForwardVector = CurrentRotation.Vector();

	// Random distance (e.g., 300 units) to search for a patrol point in front of the character
	float PatrolDistance = 300.0f;

	bool bValidPatrolPointFound = false;

	int32 TryFindPoints = 0;
	int32 MaxTries = 10;

	// Try to find a valid patrol point (loop until either 10 tries or a valid point is found)
	while (!bValidPatrolPointFound)
	{
		TryFindPoints++;

		// Find a random offset within the specified angle
		float RandomAngle = FMath::RandRange(-MaxPatrolAngle, MaxPatrolAngle);
		FRotator RandomRotation = FRotator(0, RandomAngle, 0); // Random Yaw Rotation based on angle
		FVector PatrolDirection = RandomRotation.RotateVector(ForwardVector); // Rotate the forward vector by the random angle

		// Calculate the patrol point by applying the direction and distance
		PatrolPoint = CurrentLocation + PatrolDirection * PatrolDistance;

		// Ensure the patrol point is within the navigation mesh
		FNavLocation ValidPatrolPoint;
		UNavigationSystemV1* NavSystem = UNavigationSystemV1::GetCurrent(GetWorld());

		if (NavSystem)
		{
			// Debugging: Show the patrol point being generated
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Generated Patrol Point: %s"), *PatrolPoint.ToString()));

			// Try projecting the point onto the navigation system
			if (NavSystem->ProjectPointToNavigation(PatrolPoint, ValidPatrolPoint))
			{
				// We have a valid patrol point, set it and exit the loop
				PatrolPoint = ValidPatrolPoint.Location;
				bValidPatrolPointFound = true;

				// Debugging: Show that a valid patrol point was found
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Patrol point accessible: %s"), *PatrolPoint.ToString()));
			}
			else
			{
				// If no valid patrol point is found, log and retry
				GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Patrol point not accessible, retrying...")));
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("NavSystem is NULL!"));
			break; // Exit the loop if NavSystem is null
		}

		// Exit if we're stuck and fail after max tries
		if (TryFindPoints > MaxTries)
		{
			PatrolDistance -= 50.f; // Reduce the distance if too many tries fail
			MaxPatrolAngle += 10.f; // Slightly increase the angle range after max attempts
			TryFindPoints = 0; // Reset the try count
			UE_LOG(LogTemp, Warning, TEXT("Failed to find a valid patrol point after %d attempts."), TryFindPoints);
			if (PatrolDistance < 100.f)  // If distance gets too small, we break out of the loop
			{
				break;
			}
		}
	}

	// If we found a valid patrol point, move to it
	if (bValidPatrolPointFound)
	{
		// Debugging: Log the final patrol point
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Patrol point accessible: %s"), *PatrolPoint.ToString()));
		return;
	}
	else
	{
		// If we couldn't find a valid patrol point, log the failure
		UE_LOG(LogTemp, Warning, TEXT("Failed to find a valid patrol point after 10 attempts"));
	}
}

void AGOClownAIController::MoveToPatrolPoint()
{
	DrawDebugSphere(GetWorld(), PatrolPoint, 10.0f, 10, FColor::Green, false, 10.0f);

	if (!PatrolPoint.IsZero())
	{
		// Start smooth rotation towards the patrol point
		MoveToLocationSmoothly(PatrolPoint); // Move to the patrol point with smooth rotation
	}
}

void AGOClownAIController::MoveToLocationSmoothly(const FVector& TargetLocation)
{
	// Early exit if no patrol point is set
	if (PatrolPoint.IsZero())
	{
		return;
	}

	FVector CurrentLocation = GetPawn()->GetActorLocation();
	FRotator CurrentRotation = GetPawn()->GetActorRotation();

	// Calculate the direction to the patrol point
	FVector DirectionToTarget = TargetLocation - CurrentLocation;
	DirectionToTarget.Z = 0; // Ignore Z (vertical) component

	// Calculate target rotation
	FRotator TargetRotation = DirectionToTarget.Rotation();

	// Interpolate rotation smoothly with Slerp
	FQuat CurrentQuat = FQuat(CurrentRotation);
	FQuat TargetQuat = FQuat(TargetRotation);

	// Perform slerp to smoothly rotate towards target rotation
	FQuat SmoothedRotation = FQuat::Slerp(CurrentQuat, TargetQuat, RotationSpeed * GetWorld()->GetDeltaSeconds());

	// Apply the smoothed rotation
	GetPawn()->SetActorRotation(SmoothedRotation);

	// Calculate the distance and yaw difference between current and target rotation
	float YawDifference = FMath::Abs(CurrentRotation.Yaw - TargetRotation.Yaw);

	// Ensure yaw difference is within 180 degrees for smooth turning
	if (YawDifference > 180.0f)
	{
		YawDifference = 360.0f - YawDifference;
	}

	// If sufficiently close to the target location and facing the target rotation, start moving
	if (YawDifference < 5.0f)
	{
		MoveToLocation(TargetLocation); // This triggers the AI to move towards the target location
	}
}

bool AGOClownAIController::HasReachedPatrolPoint(float ReachThreshold)
{
	FVector CurrentLocation = GetPawn()->GetActorLocation();
	float DistanceToPatrolPoint = FVector::Dist(CurrentLocation, PatrolPoint);

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