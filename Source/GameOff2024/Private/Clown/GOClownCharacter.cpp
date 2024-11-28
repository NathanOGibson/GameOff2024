// Copyright Nah Studios


#include "Clown/GOClownCharacter.h"
#include "GameFramework/Controller.h"
#include "Clown/GOClownAIController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/BillboardComponent.h"

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
	ClownAIController->SetCachedDistance(CachedDistance);

	ClownAIController->SetInactive();
	bAIActive = false;
}

void AGOClownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bAIActive) return;

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

	case EClownState::ECS_Activated:
		HandleActiveatedState();
		break;

	case EClownState::ECS_GetSearchPoint:
		HandlGetSearchPointState();
		break;

	case EClownState::ECS_Search:
		HandleSearchState();
		break;

	case EClownState::ECS_Retreat:
		HandleRetreatState();
		break;

	case EClownState::ECS_Jumpscare:
		HandleJumpscareState();
		break;
	}
}

void AGOClownCharacter::SetControllerInactive()
{
	ClownAIController->SetInactive();
	ClownState = EClownState::ECS_Idle;
	bAIActive = false;
}

void AGOClownCharacter::SetControllerActive(FVector NewCharacterLocation, FRotator NewCharacterRotation)
{
	ClownAIController->SetActive(NewCharacterLocation, NewCharacterRotation);
	ClownState = EClownState::ECS_Activated;
	bAIActive = true;
}

void AGOClownCharacter::HandleIdleState()
{
	// Set movement speed to idle movement speed
	SetCharacterSpeed(0.f);


	// Delay to transition into get patrol point state
	IdleDelay(IdleDelayAmount);

	// Transition to Chase State if player detected
	if (CheckPlayerWithinDetectionRange()) ClownState = EClownState::ECS_Chase;
}

void AGOClownCharacter::HandleGetPatrolPointState()
{
	// Transition to Chase State if player detected
	if (CheckPlayerWithinDetectionRange())
	{
		ClownState = EClownState::ECS_Chase;
		return;
	}

	// Get Patrol point
	PatrolPoint = ClownAIController->GetPatrolPoint();

	// Transition to Patrol state if Patrol point found
	if (PatrolPoint != FVector::ZeroVector)
	{
		ClownState = EClownState::ECS_Patrol;
		return;
	}

	// If no patrol point found, adjust patrol settings
	ClownAIController->ResetPatrolSettings();


}

void AGOClownCharacter::HandlePatrolState()
{
	// Set movement speed to patrol movement speed
	SetCharacterSpeed(PatrolMovementSpeed);

	// Check if patrol point reached
		//if (ClownAIController->HasReachedPatrolPoint(DistanceThreshold))
	if (ClownAIController->HasReachedLocation(PatrolPoint, DistanceThreshold))
	{
		ClownState = EClownState::ECS_Idle;
	}
	else
	{
		ClownAIController->MoveToSplinePoints();
		//ClownAIController->MoveToPatrolPoint();
	}

	// Transition to Chase State if player detected
	if (CheckPlayerWithinDetectionRange()) ClownState = EClownState::ECS_Chase;
}

void AGOClownCharacter::HandleChaseState()
{
	// Transition to Jumpscare State if player is within jumpscare range
	if (IsPlayerWithinJumpscareRange()) ClownState = EClownState::ECS_Jumpscare;

	// Set movement speed to chase movement speed
	SetCharacterSpeed(ChaseMovementSpeed);

	// Chase the player
	ClownAIController->ChasePlayer();

	if (!CheckPlayerWithinDetectionRange()) ChaseDelay(ChaseDelayAmount);
}

void AGOClownCharacter::HandleActiveatedState()
{
	// Transition to Jumpscare State if player is within jumpscare range
	if (IsPlayerWithinJumpscareRange()) ClownState = EClownState::ECS_Jumpscare;

	// Transition to Chase State if player detected
	if (CheckPlayerWithinDetectionRange()) ClownState = EClownState::ECS_Chase;

	// Set movement speed to chase movement speed
	SetCharacterSpeed(ActiveatedMovementSpeed);

	// Chase the player
	ClownAIController->ChasePlayer();

	// Transition to Chase State if player detected
	ActiveatedDelay(ActiveatedDelayAmount);
}

void AGOClownCharacter::HandlGetSearchPointState()
{
	// cache most recent player location
	SearchPoint = ClownAIController->GetSearchPoint();

	// Transition to search state if search point found
	if (SearchPoint != FVector::ZeroVector) ClownState = EClownState::ECS_Search;
}

void AGOClownCharacter::HandleSearchState()
{
	// Set movement speed to search movement speed
	SetCharacterSpeed(SearchMovementSpeed);

	// Check if search point reached
		//if (ClownAIController->HasReachedSearchPoint(DistanceThreshold))
	if (ClownAIController->HasReachedLocation(SearchPoint, DistanceThreshold))
	{
		ClownState = EClownState::ECS_Idle;
	}
	else
	{
		ClownAIController->MoveToSplinePoints();
		//ClownAIController->MoveToSearchPoint();
	}

	// Transition to Chase State if player detected
	if (CheckPlayerWithinDetectionRange()) ClownState = EClownState::ECS_Chase;
}

void AGOClownCharacter::HandleRetreatState()
{
	SetCharacterSpeed(RetreatMovementSpeed);
	ClownAIController->MoveToRetreatPoint(RetreatPoint);
}

void AGOClownCharacter::HandleJumpscareState()
{
	JumpscarePlayer();
}


void AGOClownCharacter::SetToRetreatState(FVector NewRetreatLocation)
{
	RetreatPoint = NewRetreatLocation;
	ClownState = EClownState::ECS_Retreat;
}

bool AGOClownCharacter::HasFinishedRetreating()
{
	return ClownAIController->HasReachedLocation(RetreatPoint, DistanceThreshold);
}

void AGOClownCharacter::SetCharacterSpeed(float Speed)
{
	if (UCharacterMovementComponent* MovementComponent = GetCharacterMovement())
	{
		MovementComponent->MaxWalkSpeed = Speed;
	}
}

bool AGOClownCharacter::CheckPlayerWithinDetectionRange()
{
	if (!Player || !ClownAIController) return false;

	// Get locations and calculate distance
	const FVector PlayerLocation = Player->GetActorLocation();
	const FVector ClownLocation = GetActorLocation();
	const float DistanceToPlayer = FVector::Dist(ClownLocation, PlayerLocation);

	// Always decay clown detection
	AdjustClownDetection(DetectionDecayRate);

	// Draw detection range
	//DebugDetectionRange();

	if (!IsPlayerWithinDetectionRange(DistanceToPlayer)) return false;

	if (IsPlayerWithinAgroRange(DistanceToPlayer)) return true;

	if (!IsPlayerWithinDetectionAngle(PlayerLocation)) return false;

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
		//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::Green, TEXT("Outside detection range"));
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
		return false;
	}

	return true;
}

bool AGOClownCharacter::IsPlayerWithinAgroRange(float DistanceToPlayer)
{
	if (DistanceToPlayer <= DetectionAgroRange)
	{
		ClownDetection = DetectionThreshold;
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
		//DrawDebugLine(GetWorld(), ClownLocation, EndLocation, FColor::Red, false, 1.0f, 0, 2.0f);
		return HitResult.GetActor() == Player;
	}

	return false;
}

void AGOClownCharacter::AdjustClownDetectionBasedOnPlayerMovement()
{
	if (!CheckPlayerIsMoving())
	{
		AdjustClownDetection(DetectionNotWalkingIncreaseRate);
	}
	else if (CheckPlayerIsCrouching())
	{
		AdjustClownDetection(DetectionCrouchingIncreaseRate);
	}
	else if (CheckPlayerIsSprinting())
	{
		AdjustClownDetection(DetectionSprintingIncreaseRate);
	}
	else
	{
		AdjustClownDetection(DetectionWalkingIncreaseRate);
	}
}

bool AGOClownCharacter::IsPlayerWithinJumpscareRange()
{
	if (!Player || !ClownAIController) return false;

	// Get locations and calculate distance
	const FVector PlayerLocation = Player->GetActorLocation();
	const FVector ClownLocation = GetActorLocation();
	const float DistanceToPlayer = FVector::Dist(ClownLocation, PlayerLocation);

	if (DistanceToPlayer <= JumpscareRange)
	{
		ClownDetection = DetectionThreshold;
		return true;
	}

	return false;
}
