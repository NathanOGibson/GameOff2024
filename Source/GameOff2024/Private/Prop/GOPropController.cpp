// Copyright Nah Studios


#include "Prop/GOPropController.h"
#include "Prop/GOProp.h"
#include "Kismet/GameplayStatics.h"

AGOPropController::AGOPropController()
{
	PrimaryActorTick.bCanEverTick = true;
	ControllerSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ControllerSceneComponent"));
	ControllerSceneComponent->SetupAttachment(RootComponent);
	SetRootComponent(ControllerSceneComponent);
}

void AGOPropController::BeginPlay()
{
	Super::BeginPlay();
	GetPropReferences();
	AssignChosenAndExtraProps();
}

void AGOPropController::GetPropReferences()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGOProp::StaticClass(), OutActors);

	for (AActor* Actor : OutActors)
	{
		AGOProp* Prop = Cast<AGOProp>(Actor);
		AllProps.Add(Prop);
	}
}

TObjectPtr<AGOProp> AGOPropController::PickProp()
{
	int32 RandomIndex = FMath::RandRange(0, AllProps.Num() - 1);
	return AllProps[RandomIndex];
}

// Will be called on the Booth Entity
void AGOPropController::AssignChosenAndExtraProps()
{
	ChosenProp = PickProp();
	while (ExtraProps.Num() < 2)
	{
		AGOProp* Prop = PickProp();
		if (!ExtraProps.Contains(Prop) && Prop != ChosenProp)
		{
			ExtraProps.Add(Prop);
		}
	}
}

void AGOPropController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

