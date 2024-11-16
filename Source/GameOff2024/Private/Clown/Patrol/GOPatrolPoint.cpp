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

void AGOPatrolPoint::SetToMaterialActive()
{
	SetMaterial(ActiveMaterial);
}

void AGOPatrolPoint::SetMaterialToDisabled()
{
	SetMaterial(DisabledMaterial);
}

void AGOPatrolPoint::SetMaterialToCached()
{
	SetMaterial(CachedMaterial);
}

void AGOPatrolPoint::SetMaterial(TObjectPtr<UMaterial> NewMaterial)
{
	PatrolPointMesh->SetMaterial(0, NewMaterial);
}
