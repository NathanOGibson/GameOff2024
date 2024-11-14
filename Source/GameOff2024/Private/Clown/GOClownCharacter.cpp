// Copyright Nah Studios


#include "Clown/GOClownCharacter.h"
#include "GameFramework/Controller.h"
#include "Clown/GOClownAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"

AGOClownCharacter::AGOClownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initial state is patrol
	ClownState = EClownState::ECS_Idle;
}

void AGOClownCharacter::BeginPlay()
{
	Super::BeginPlay();

	ClownAIController = Cast<AGOClownAIController>(GetController());
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	ClownAIController->SetPatrolDistance(PatrolDistance);
}

void AGOClownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);




	// SWITCH FOR SETTING SPEED


	//if (FVector::Dist(GetActorLocation(), Player->GetActorLocation()) <= DetectionRange)
	//{
	//	// Switch to chase state if close enough to the player
	//	ClownState = EClownState::ECS_Chase;
	//}
	//else if (ClownState != EClownState::ECS_Patrol || ClownState != EClownState::ECS_Idle || ClownState != EClownState::ECS_GetPatrolPoint)
	//{
	//	// Patrol if far away from the player
	//	ClownState = EClownState::ECS_GetPatrolPoint;
	//}
	switch (ClownState)
	{
	case EClownState::ECS_Idle:
		HandleIdleState();
		break;

	case EClownState::ECS_GetPatrolPoint:
		HandleGetPatrolPointState();
		break;

	case EClownState::ECS_Patrol:
		HandlePatrolState();
		break;

	case EClownState::ECS_Chase:
		HandleChaseState();
		break;

	case EClownState::ECS_GetSearchPoint:
		HandlGetSearchPointState();
		break;

	case EClownState::ECS_Search:
		HandleSearchState();
		break;

	case EClownState::ECS_Jumpscare:
		HandleJumpscareState();
		break;
	}
}

void AGOClownCharacter::JumpscarePlayer()
{
	if (Player && FVector::Dist(GetActorLocation(), Player->GetActorLocation()) <= JumpscareRange)
	{
		// Logic to trigger the jumpscare animation or event
		// E.g., play jumpscare animation, sound, and possibly reduce the player's health
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Jumpscare triggered!"));
	}
}

void AGOClownCharacter::IdleDelay()
{
	ClownState = EClownState::ECS_GetPatrolPoint;
}

void AGOClownCharacter::SetCharacterSpeed(float Speed)
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = Speed;
	}
}

// STATE FUNCTIONS
void AGOClownCharacter::HandleIdleState()
{
	// Do nothing

	// Check if timer finished (GET PATROL POINT) IdleDelayAmount
	DelayEvent(IdleDelayAmount);

	// Check for player detection (CHASE)
	if (CheckPlayerWithinDetectionRange()) ClownState = EClownState::ECS_Chase;
}

void AGOClownCharacter::HandleGetPatrolPointState()
{
	// Get Patrol point
	FVector NewMovePoint;
	NewMovePoint = ClownAIController->GetPatrolPoint();

	// Check Patrol point found (PATROL)
	if (NewMovePoint != FVector::ZeroVector) ClownState = EClownState::ECS_Patrol;
	ClownAIController->IncraseMaxPatrolAngle();

	// Check for player detection (CHASE)
	if (CheckPlayerWithinDetectionRange()) ClownState = EClownState::ECS_Chase;
}

void AGOClownCharacter::HandlePatrolState()
{
	SetCharacterSpeed(PatrolMovementSpeed);

	ClownAIController->ResetMaxPatrolAngle();
	// Move towards the patrol point
	ClownAIController->MoveToPatrolPoint();

	// Check if reached patrol point (IDLE)
	if (ClownAIController->HasReachedPatrolPoint(100.f))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Patrol point reached!"));
		ClownState = EClownState::ECS_Idle;
	}

	// Check for player detection (CHASE)
	if (CheckPlayerWithinDetectionRange()) ClownState = EClownState::ECS_Chase;
}

void AGOClownCharacter::HandleChaseState()
{
	SetCharacterSpeed(ChaseMovementSpeed);

	// Chase the player
	ClownAIController->ChasePlayer();

	// Check if player outside player detection range (SEARCH)
	if (!CheckPlayerWithinDetectionRange()) ClownState = EClownState::ECS_GetSearchPoint;
	// Check if player inside jumpscare range (JUMPSCARE)
}

void AGOClownCharacter::HandlGetSearchPointState()
{
	// cache most recent player location
	FVector NewSearchPoint;
	NewSearchPoint = ClownAIController->GetSearchPoint();
	if (NewSearchPoint != FVector::ZeroVector) ClownState = EClownState::ECS_Search;

	// Check search point found (SEARCH)

}

void AGOClownCharacter::HandleSearchState()
{
	SetCharacterSpeed(SearchMovementSpeed);
	// Move to search location
	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Searching for player..."));

	// Check if timer finished (IDLE) SearchDelayAmount
	if (ClownAIController->HasReachedSearchPoint(100.f))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Red, TEXT("Patrol point reached!"));
		DelayEvent(SearchDelayAmount);
	}
	// Check for player detection (CHASE)
	if (CheckPlayerWithinDetectionRange()) ClownState = EClownState::ECS_Chase;
}

void AGOClownCharacter::HandleJumpscareState()
{
	// Play jumpscare animation
	// Play jumpscare sound
	// End game
}

bool AGOClownCharacter::CheckPlayerWithinDetectionRange()
{
	if (!Player || !ClownAIController) return false;

	// Check if the player is within the detection range
	if (FVector::Dist(GetActorLocation(), Player->GetActorLocation()) <= DetectionRange)
	{
		// Get direction from clown to player
		FVector DirectionToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();

		// Get clown's forward vector
		FVector ClownForward = GetActorForwardVector().GetSafeNormal();

		// Calculate the dot product between forward vector and direction to player
		float DotProduct = FVector::DotProduct(ClownForward, DirectionToPlayer);

		// Calculate the angle from the dot product
		float AngleDegrees = FMath::Acos(DotProduct) * (180.0f / PI);

		// Define the detection angle (in degrees)
		float HalfDetectionAngle = DetectionAngle / 2.0f;

		// Debug: Draw detection angle lines
		FVector RightDetectionLine = ClownForward.RotateAngleAxis(HalfDetectionAngle, FVector::UpVector);
		FVector LeftDetectionLine = ClownForward.RotateAngleAxis(-HalfDetectionAngle, FVector::UpVector);

		// Draw debug lines to visualize detection cone
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + RightDetectionLine * DetectionRange, FColor::Green, false, 1.0f, 0, 2.0f);
		DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + LeftDetectionLine * DetectionRange, FColor::Green, false, 1.0f, 0, 2.0f);

		// Check if player is within the detection angle (e.g., 45 degrees)
		if (AngleDegrees <= HalfDetectionAngle)
		{
			// Perform a line trace (raycast) to check for obstacles between the clown and the player
			FHitResult HitResult;
			FVector StartLocation = GetActorLocation();
			FVector EndLocation = Player->GetActorLocation();
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);  // Ignore the clown itself in the line trace

			// Draw a debug line for the line trace
			DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, 1.0f, 0, 2.0f);

			// Line trace to check if anything blocks the line of sight
			bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, CollisionParams);

			// Debug: Print out the result of the line trace
			if (bHit)
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Hit!"));
				//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Hit Actor: %s"), *HitResult.GetActor()->GetName()));

				// Check if the hit actor is the player
				if (HitResult.GetActor() == Player)
				{
					//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Player is in sight!"));
					return true;
				}
			}
			else
			{
				//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("No hit detected."));
			}
		}
	}
	return false;
}
