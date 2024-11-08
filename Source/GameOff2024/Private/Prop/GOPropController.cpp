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
	// Array to hold all actors of class AGOProp found in the world
	TArray<AActor*> OutActors;

	// Get all actors of class AGOProp in the current world and store them in OutActors
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGOProp::StaticClass(), OutActors);

	// Loop through all found actors and cast them to AGOProp before adding them to AllProps
	for (AActor* Actor : OutActors)
	{
		AGOProp* Prop = Cast<AGOProp>(Actor);
		if (Prop) AllProps.Add(Prop);  // Add valid props to the array
	}
}

TObjectPtr<AGOProp> AGOPropController::PickProp()
{
	// Pick a random index from the AllProps array
	int32 RandomIndex = FMath::RandRange(0, AllProps.Num() - 1);

	// Return the randomly selected prop
	return AllProps[RandomIndex];
}

// Will be called on the Booth Entity
void AGOPropController::AssignChosenAndExtraProps()
{
	// Pick the main chosen prop
	ChosenProp = PickProp();

	// Loop until two unique extra props are selected
	while (ExtraProps.Num() < 2)
	{
		AGOProp* Prop = PickProp();

		// Add the prop to ExtraProps if it's not already there and isn't the chosen prop
		if (!ExtraProps.Contains(Prop) && Prop != ChosenProp) ExtraProps.Add(Prop);
	}
}

void AGOPropController::ResetPropPositions()
{
	// Loop through all props and reset their location and rotation
	for (AGOProp* Prop : AllProps)
	{
		if (Prop)
		{
			// Set the prop's location to the controller's location
			Prop->SetActorLocation(GetActorLocation());

			// Set the prop's rotation to a default (zeroed) rotation
			Prop->SetActorRotation(FRotator(0.f, 0.f, 0.f));
		}
	}
}

void AGOPropController::SetSpawnPoints()
{
	// Get all child components of this actor
	TArray<USceneComponent*> ChildrenComponents;
	GetComponents(ChildrenComponents);

	// Remove the root component (index 0) to focus only on the actual children
	if (ChildrenComponents.Num() > 0) ChildrenComponents.RemoveAt(0);

	// Variable to store the current selected child (spawn point)
	USceneComponent* CurrentChild = nullptr;

	// Randomly select an initial spawn point from the children
	int32 RandomIndex = FMath::RandRange(0, ChildrenComponents.Num() - 1);
	CurrentChild = ChildrenComponents[RandomIndex];

	// Display the name of the initial spawn point for debugging

	// Add the randomly selected spawn point to the list of chosen spawn locations
	ChildrenSpawnLocations.Add(CurrentChild);
	// Remove the selected child from the available components
	ChildrenComponents.Remove(CurrentChild);

	// Continue to add spawn points until we have 6, or there are no more children to choose from
	while (ChildrenSpawnLocations.Num() < 6 && CurrentChild != nullptr && ChildrenComponents.Num() > 0)
	{
		// Get the spawn point furthest from the already chosen points
		CurrentChild = GetFurthestSpawnPoint(ChildrenSpawnLocations, ChildrenComponents);

		// Display the name of the newly selected spawn point for debugging

		// Add the furthest spawn point to the list and remove it from the available components
		ChildrenSpawnLocations.Add(CurrentChild);
		ChildrenComponents.Remove(CurrentChild);
	}
}

USceneComponent* AGOPropController::GetFurthestSpawnPoint(TArray<USceneComponent*>& StartPoints, TArray<USceneComponent*>& ChildrenPoints)
{
	USceneComponent* FurthestSpawnPoint = nullptr;  // Variable to store the furthest spawn point
	float FurthestDistance = -1.f;                  // Variable to track the furthest distance found
	float SkipChance = 0.2f;                        // 20% chance to randomly skip a spawn point

	// Loop through each potential spawn point in ChildrenPoints
	for (USceneComponent* SpawnPoint : ChildrenPoints)
	{
		// Random chance to skip the current spawn point (based on SkipChance)
		if (FMath::FRand() < SkipChance)
		{
			continue;  // Skip this iteration if the random chance is below the threshold
		}

		float ClosestDistanceToStart = FLT_MAX;  // Initialize to a large value for distance comparison

		// Find the closest distance from the current spawn point to any of the start points
		for (USceneComponent* StartPoint : StartPoints)
		{
			// Calculate the distance between the start point and spawn point
			float DistanceToStart = FVector::Dist(StartPoint->GetComponentLocation(), SpawnPoint->GetComponentLocation());

			// Keep track of the closest distance between this spawn point and any start point
			if (DistanceToStart < ClosestDistanceToStart)
			{
				ClosestDistanceToStart = DistanceToStart;
			}
		}

		// If this spawn point's closest distance is further than the current furthest distance, update it
		if (ClosestDistanceToStart > FurthestDistance)
		{
			FurthestSpawnPoint = SpawnPoint;  // Set this spawn point as the new furthest
			FurthestDistance = ClosestDistanceToStart;  // Update the furthest distance
		}
	}

	return FurthestSpawnPoint;  // Return the spawn point that is furthest from the start points
}

void AGOPropController::SetPropPositions()
{
	// Define a minimum distance that extra props should maintain between each other
	const float MinDistanceBetweenExtraProps = 2000.f;

	// Start by adding the two extra props to the placement list
	TArray<TObjectPtr<AGOProp>> PlacementProps;
	PlacementProps.Add(ExtraProps[0]);
	PlacementProps.Add(ExtraProps[1]);

	// Pick random locations for extra props ensuring they are far apart
	USceneComponent* FirstExtraPropLocation = nullptr;

	// Randomly select a spawn location for the first extra prop
	int32 RandomExtraIndex = FMath::RandRange(0, ChildrenSpawnLocations.Num() - 1);
	FirstExtraPropLocation = ChildrenSpawnLocations[RandomExtraIndex];

	// Place the first extra prop at the selected location
	PlacementProps[0]->SetActorLocation(FirstExtraPropLocation->GetComponentLocation());
	PlacementProps[0]->SetActorRotation(FirstExtraPropLocation->GetComponentRotation());
	ChildrenSpawnLocations.RemoveAt(RandomExtraIndex);

	// Find a location for the second extra prop that is far enough from the first
	USceneComponent* SecondExtraPropLocation = nullptr;
	for (USceneComponent* PotentialLocation : ChildrenSpawnLocations)
	{
		float Distance = FVector::Dist(FirstExtraPropLocation->GetComponentLocation(), PotentialLocation->GetComponentLocation());

		// Ensure the distance between the two locations is greater than the defined minimum
		if (Distance > MinDistanceBetweenExtraProps)
		{
			SecondExtraPropLocation = PotentialLocation;
			break;
		}
	}

	// If a valid second location is found, place the second extra prop there
	if (SecondExtraPropLocation)
	{
		GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Green, FString::Printf(TEXT("LargeDist")));
		PlacementProps[1]->SetActorLocation(SecondExtraPropLocation->GetComponentLocation());
		PlacementProps[1]->SetActorRotation(SecondExtraPropLocation->GetComponentRotation());
		ChildrenSpawnLocations.Remove(SecondExtraPropLocation);
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 100.0f, FColor::Green, FString::Printf(TEXT("Rand")));
		// If no valid far location is found, fallback to a random remaining location
		int32 FallbackIndex = FMath::RandRange(0, ChildrenSpawnLocations.Num() - 1);
		USceneComponent* FallbackLocation = ChildrenSpawnLocations[FallbackIndex];
		PlacementProps[1]->SetActorLocation(FallbackLocation->GetComponentLocation());
		PlacementProps[1]->SetActorRotation(FallbackLocation->GetComponentRotation());
		ChildrenSpawnLocations.RemoveAt(FallbackIndex);
	}

	// Now, fill the remaining slots with other unique props
	while (PlacementProps.Num() < 6)
	{
		AGOProp* Prop = PickProp();

		// Ensure that the prop is not already chosen, not extra, and not in PlacementProps
		if (!ExtraProps.Contains(Prop) && Prop != ChosenProp && !PlacementProps.Contains(Prop))
		{
			PlacementProps.Add(Prop);
		}
	}

	// Place remaining props at available spawn locations
	for (int32 i = 0; i < ChildrenSpawnLocations.Num(); i++)
	{
		if (PlacementProps.Num() > 0)
		{
			USceneComponent* Child = ChildrenSpawnLocations[i];

			// Pick a random prop from the placement props array
			int32 RandomIndex = FMath::RandRange(0, PlacementProps.Num() - 1);
			TObjectPtr<AGOProp> Prop = PlacementProps[RandomIndex];

			// Place the prop at the selected spawn location
			Prop->SetActorLocation(Child->GetComponentLocation());
			Prop->SetActorRotation(Child->GetComponentRotation());

			// Remove the selected prop from the placement list
			PlacementProps.RemoveAt(RandomIndex);
		}
	}
}

void AGOPropController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

