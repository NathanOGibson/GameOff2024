// Copyright Nah Studios


#include "Clown/GOClownCharacter.h"
#include "GameFramework/Controller.h"
#include "Clown/GOClownAIController.h"
#include "Kismet/GameplayStatics.h"

AGOClownCharacter::AGOClownCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Initial state is patrol
	ClownState = EClownState::ECS_GetPatrolPoint;
}

void AGOClownCharacter::BeginPlay()
{
	Super::BeginPlay();

	ClownAIController = Cast<AGOClownAIController>(GetController());
	Player = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
}

void AGOClownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!Player || !ClownAIController) return;

	if (FVector::Dist(GetActorLocation(), Player->GetActorLocation()) <= DetectionRange)
	{
		// Switch to chase state if close enough to the player
		ClownState = EClownState::ECS_Chase;
	}
	//else if (ClownState != EClownState::ECS_Patrol || ClownState != EClownState::ECS_Idle || ClownState != EClownState::ECS_GetPatrolPoint)
	//{
	//	// Patrol if far away from the player
	//	ClownState = EClownState::ECS_GetPatrolPoint;
	//}

	switch (ClownState)
	{
	case EClownState::ECS_Idle:
		ClownState = EClownState::ECS_GetPatrolPoint;
		break;

	case EClownState::ECS_GetPatrolPoint:
		ClownAIController->GetPatrolPoint();
		ClownState = EClownState::ECS_Patrol;
		break;

	case EClownState::ECS_Patrol:
		ClownAIController->MoveToPatrolPoint();
		if (ClownAIController->HasReachedPatrolPoint(100.f))
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Patrol point reached!"));
			ClownState = EClownState::ECS_Idle;
		}
		break;

	case EClownState::ECS_Chase:
		ClownAIController->ChasePlayer();
		break;

	case EClownState::ECS_Jumpscare:
		JumpscarePlayer();
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
