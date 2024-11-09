// Copyright Nah Studios


#include "Clown/GOBooth.h"
#include "Prop/GOPropController.h"
#include "Kismet/GameplayStatics.h"

AGOBooth::AGOBooth()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGOBooth::InitialiseReferences()
{
	PropController = Cast<AGOPropController>(UGameplayStatics::GetActorOfClass(GetWorld(), AGOPropController::StaticClass()));
}

void AGOBooth::BeginPlay()
{
	Super::BeginPlay();
}

void AGOBooth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

