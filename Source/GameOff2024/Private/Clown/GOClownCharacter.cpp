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
	IdleDelay(IdleDelayAmount);

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
	ClownAIController->AdjustPatrolSettings();

	// Check for player detection (CHASE)
	if (CheckPlayerWithinDetectionRange()) ClownState = EClownState::ECS_Chase;
}

void AGOClownCharacter::HandlePatrolState()
{
	SetCharacterSpeed(PatrolMovementSpeed);

	ClownAIController->ResetPatrolSettings();
	// Move towards the patrol point

	// Check if reached patrol point (IDLE)
	if (ClownAIController->HasReachedPatrolPoint(100.f))
	{
		PatrolDelay(PatrolDelayAmount);
	}
	else
	{
		ClownAIController->MoveToPatrolPoint();
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
	if (!CheckPlayerWithinDetectionRange())
	{
		ChaseDelay(ChaseDelayAmount);
	}
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
		SearchDelay(SearchDelayAmount);
	}
	else
	{
		ClownAIController->MoveToSearchPoint();
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

	// Calculate distance and direction to the player
	const FVector PlayerLocation = Player->GetActorLocation();
	const FVector ClownLocation = GetActorLocation();
	const float DistanceToPlayer = FVector::Dist(ClownLocation, PlayerLocation);

	// Always decay clown detection
	AdjustClownDetection(DetectionDecayRate);

	DebugDetectionRange();

	if (!IsPlayerWithinDetectionRange(DistanceToPlayer)) return false;

	if (!IsPlayerWithinDetectionAngle(PlayerLocation)) return false;

	if (IsPlayerWithinAgroRange(DistanceToPlayer)) return true;

	if (IsPlayerInLineOfSight(PlayerLocation, ClownLocation))
	{
		AdjustClownDetectionBasedOnPlayerMovement();

		// If detection reaches threshold, switch to chase state
		if (ClownDetection >= DetectionThreshold) return true;
	}

	return false;
}

void AGOClownCharacter::AdjustClownDetection(float DetectionAdjustmentAmount)
{
	float DetectionIncreaseAmount = ClownDetection + DetectionAdjustmentAmount;
	ClownDetection = FMath::Clamp(DetectionIncreaseAmount, 0.0f, DetectionThreshold);
}

void AGOClownCharacter::DebugDetectionRange()
{
	FVector ClownForward = GetActorForwardVector().GetSafeNormal();
	float HalfDetectionAngle = DetectionAngle / 2.0f;

	// Debug: Draw detection angle lines
	FVector RightDetectionLine = ClownForward.RotateAngleAxis(HalfDetectionAngle, FVector::UpVector);
	FVector LeftDetectionLine = ClownForward.RotateAngleAxis(-HalfDetectionAngle, FVector::UpVector);

	// Draw debug lines to visualize detection cone
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + RightDetectionLine * DetectionQueryRange, FColor::Green, false, 1.0f, 0, 2.0f);
	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + LeftDetectionLine * DetectionQueryRange, FColor::Green, false, 1.0f, 0, 2.0f);
}

bool AGOClownCharacter::IsPlayerWithinDetectionRange(float DistanceToPlayer)
{
	if (DistanceToPlayer > DetectionQueryRange)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, TEXT("Outside detection range"));
		return false;
	}

	return true;
}

bool AGOClownCharacter::IsPlayerWithinDetectionAngle(FVector PlayerLocation)
{
	const FVector DirectionToPlayer = (Player->GetActorLocation() - GetActorLocation()).GetSafeNormal();
	const float AngleDegrees = FMath::RadiansToDegrees(FMath::Acos(FVector::DotProduct(GetActorForwardVector().GetSafeNormal(), DirectionToPlayer)));

	if (AngleDegrees > DetectionAngle / 2.0f)
	{
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, TEXT("Outside detection angle"));
		return false;
	}

	return true;
}

bool AGOClownCharacter::IsPlayerWithinAgroRange(float DistanceToPlayer)
{
	if (DistanceToPlayer <= DetectionAgroRange)
	{
		ClownDetection = DetectionThreshold;
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("AGRO"));
		return true;
	}

	return false;
}

bool AGOClownCharacter::IsPlayerInLineOfSight(FVector PlayerLocation, FVector ClownLocation)
{
	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);

	const FVector EndLocation = FVector(PlayerLocation.X, PlayerLocation.Y, ClownLocation.Z);

	if (GetWorld()->LineTraceSingleByChannel(HitResult, ClownLocation, EndLocation, ECC_Visibility, CollisionParams))
	{
		DrawDebugLine(GetWorld(), ClownLocation, EndLocation, FColor::Red, false, 1.0f, 0, 2.0f);
		return HitResult.GetActor() == Player;
	}

	return false;
}

void AGOClownCharacter::AdjustClownDetectionBasedOnPlayerMovement()
{
	if (!CheckPlayerIsMoving())
	{
		AdjustClownDetection(DetectionNotWalkingIncreaseRate);
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("NOT WALKING"));
	}
	else if (CheckPlayerIsCrouching())
	{
		AdjustClownDetection(DetectionCrouchingIncreaseRate);
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("CROUCHING"));
	}
	else if (CheckPlayerIsSprinting())
	{
		AdjustClownDetection(DetectionSprintingIncreaseRate);
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("SPRINTING"));
	}
	else
	{
		AdjustClownDetection(DetectionWalkingIncreaseRate);
		GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Red, TEXT("WALKING"));
	}
}





void AGOClownCharacter::TempFunc()
{

}