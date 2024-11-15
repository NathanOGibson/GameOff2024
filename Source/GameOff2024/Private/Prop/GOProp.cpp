// Copyright Nah Studios


#include "Prop/GOProp.h"

AGOProp::AGOProp()
{
	PrimaryActorTick.bCanEverTick = true;
	PropSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("PropSceneComponent"));
	PropSceneComponent->SetupAttachment(RootComponent);
	SetRootComponent(PropSceneComponent);

	PropMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PropMesh"));
	PropMesh->SetupAttachment(RootComponent);
}

void AGOProp::BeginPlay()
{
	Super::BeginPlay();

}