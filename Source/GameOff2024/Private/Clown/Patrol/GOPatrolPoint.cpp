// Copyright Nah Studios


#include "Clown/Patrol/GOPatrolPoint.h"

AGOPatrolPoint::AGOPatrolPoint()
{
	PrimaryActorTick.bCanEverTick = true;
	// Create default scene component
	PatrolPointSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PatrolPointSceneComponent"));
	PatrolPointSceneComponent->SetupAttachment(RootComponent);
	SetRootComponent(PatrolPointSceneComponent);

	// Create default mesh
	PatrolPointMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PatrolPointMesh"));
	PatrolPointMesh->SetupAttachment(RootComponent);
}
