// Copyright Nah Studios


#include "Prop/GOPropController.h"
#include "Prop/GOProp.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"

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

	ResetPropPositions();
	SetSpawnPoints();
	SetPropPositions();
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

void AGOPropController::ResetPropPositions()
{
	for (AGOProp* Prop : AllProps)
	{
		Prop->SetActorLocation(GetActorLocation());
		Prop->SetActorRotation(FRotator(0.f, 0.f, 0.f));
	}
}

void AGOPropController::SetSpawnPoints()
{
	TArray<USceneComponent*> ChildrenComponents;
	GetComponents(ChildrenComponents);
	ChildrenComponents.RemoveAt(0);

	USceneComponent* CurrentChild = nullptr;

	int32 RandomIndex = FMath::RandRange(0, ChildrenComponents.Num() - 1);
	CurrentChild = ChildrenComponents[RandomIndex];
	GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Green, FString::Printf(TEXT("Actor Name: %s"), *CurrentChild->GetName()));
	ChildrenSpawnLocations.Add(CurrentChild);
	ChildrenComponents.Remove(CurrentChild);

	while (ChildrenSpawnLocations.Num() < 6 && CurrentChild != nullptr && ChildrenComponents.Num() > 0)
	{
		CurrentChild = GetFurthestSpawnPoint(CurrentChild, ChildrenComponents);
		GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Green, FString::Printf(TEXT("Actor Name: %s"), *CurrentChild->GetName()));
		ChildrenSpawnLocations.Add(CurrentChild);
		ChildrenComponents.Remove(CurrentChild);
	}
}

USceneComponent* AGOPropController::GetFurthestSpawnPoint(USceneComponent* StartPoint, TArray<USceneComponent*> ChildrenPoints)
{
	USceneComponent* FurthestSpawnPoint = nullptr;
	float FurthestDistance = -1.f;
	float SkipChance = 0.2f;

	for (USceneComponent* SpawnPoint : ChildrenPoints)
	{
		// Random chance to skip the object
		if (FMath::FRand() < SkipChance)
		{
			GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Red, FString::Printf(TEXT("SKIPPED")));

			continue;  // Skip this iteration if the random chance is below the threshold
		}

		float Distance = FVector::Dist(StartPoint->GetComponentLocation(), SpawnPoint->GetComponentLocation());
		if (Distance > FurthestDistance)
		{
			FurthestSpawnPoint = SpawnPoint;
			FurthestDistance = Distance;
		}
	}

	return FurthestSpawnPoint;
}

void AGOPropController::SetPropPositions()
{
	TArray<TObjectPtr<AGOProp>> PlacementProps;
	PlacementProps.Add(ExtraProps[0]);
	PlacementProps.Add(ExtraProps[1]);
	while (PlacementProps.Num() < 6)
	{
		AGOProp* Prop = PickProp();
		if (!ExtraProps.Contains(Prop) && Prop != ChosenProp && !PlacementProps.Contains(Prop))
		{
			PlacementProps.Add(Prop);
		}
	}

	for (int32 i = 0; i < ChildrenSpawnLocations.Num(); i++)
	{
		USceneComponent* Child = ChildrenSpawnLocations[i];

		if (PlacementProps.Num() > 0)
		{
			int32 RandomIndex = FMath::RandRange(0, PlacementProps.Num() - 1);
			TObjectPtr<AGOProp> Prop = PlacementProps[RandomIndex];

			//GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Green, FString::Printf(TEXT("Actor Name: %s"), *Prop.GetName()));

			Prop->SetActorLocation(Child->GetComponentLocation());
			Prop->SetActorRotation(Child->GetComponentRotation());

			PlacementProps.RemoveAt(RandomIndex);
		}
	}
}

void AGOPropController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

