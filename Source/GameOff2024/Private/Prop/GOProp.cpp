// Copyright Nah Studios


#include "Prop/GOProp.h"

AGOProp::AGOProp()
{
	PrimaryActorTick.bCanEverTick = true;
	// Create default scene component
	PropSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PropSceneComponent"));
	PropSceneComponent->SetupAttachment(RootComponent);
	SetRootComponent(PropSceneComponent);

	// Create default mesh
	PropMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PropMesh"));
	PropMesh->SetupAttachment(RootComponent);
}

void AGOProp::BeginPlay()
{
	Super::BeginPlay();

}