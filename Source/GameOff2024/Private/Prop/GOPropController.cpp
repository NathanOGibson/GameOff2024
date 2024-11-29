// Copyright Nah Studios


#include "Prop/GOPropController.h"
#include "Prop/GOProp.h"
#include "Kismet/GameplayStatics.h"
#include "Math/Vector.h"

AGOPropController::AGOPropController()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the root scene component
	ControllerSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("ControllerSceneComponent"));
	ControllerSceneComponent->SetupAttachment(RootComponent);
	SetRootComponent(ControllerSceneComponent);
}

void AGOPropController::BeginPlay()
{
	Super::BeginPlay();
	PropsSavedSpawnCount = PropsToSpawn;
}
void AGOPropController::ResetAndInitialiseProps()
{
	InitialisePropReferences();
	ResetAllPropLocations();
}

void AGOPropController::SetupPropsAndSpawns(TArray<AGOProp*>& ChosenTargetProps)
{
	// Reset the number of props to spawn
	PropsToSpawn = PropsSavedSpawnCount;

	SelectChoseProps();
	InitialiseSpawnLocations();
	SetPropLocations();

	ChosenTargetProps = ChosenProps;
}

void AGOPropController::ClearAndResetProps()
{
	ResetAllPropLocations();
	ClearPropData();
}

void AGOPropController::InitialisePropReferences()
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

void AGOPropController::SelectChoseProps()
{
	ChosenProps.Empty();

	// Ensure we have enough props to choose from
	if (AllProps.Num() < PropsToChoose)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough unique props to fill the ChosenProps list!"));
	}

	// Pick unique props up to the desired number
	while (ChosenProps.Num() < PropsToChoose && ChosenProps.Num() < AllProps.Num())
	{
		AGOProp* Prop = PickProp();

		// Check for null and ensure the prop isn't already chosen
		if (Prop && !ChosenProps.Contains(Prop))
		{
			ChosenProps.Add(Prop);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Duplicate or invalid prop encountered!"));
		}
	}

	// Log if we didn't fill up the list
	if (ChosenProps.Num() < PropsToChoose)
	{
		UE_LOG(LogTemp, Warning, TEXT("Only %d props could be selected, but %d were required."), ChosenProps.Num(), PropsToChoose);
	}
}

void AGOPropController::ClearPropData()
{
	// Reset the chosen prop list
	ChosenProps.Empty();    // Resets the pointer to the chosen prop to null

	// Clear spawn locations if needed
	ChildrenSpawnLocations.Empty(); // Clears the spawn locations array if you want to reset that too

	// Add any additional variables or arrays to clear/reset here as needed
	// ->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, TEXT("Prop data cleared"));
}

void AGOPropController::InitialiseSpawnLocations()
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
	while (ChildrenSpawnLocations.Num() < PropsToSpawn && CurrentChild != nullptr && ChildrenComponents.Num() > 0)
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
	float ChoseChance = 0.15f;                        // 15% chance to randomly skip a spawn point

	// Loop through each potential spawn point in ChildrenPoints
	for (USceneComponent* SpawnPoint : ChildrenPoints)
	{
		// Random chance to chose the current spawn point (based on ChoseChance)
		if (FMath::FRand() < ChoseChance)
		{
			return SpawnPoint;
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

void AGOPropController::ResetAllPropLocations()
{
	// Loop through all props and reset their location and rotation
	for (AGOProp* Prop : AllProps)
	{
		ResetPropLocation(Prop);
	}
}

void AGOPropController::ResetPropLocation(AGOProp* Prop)
{
	// Set the prop's location to the controller's location
	// Set the prop's rotation to a default (zeroed) rotation
	SetPropLocationAndRotation(Prop, GetActorLocation(), FRotator(0.f, 0.f, 0.f));
}

void AGOPropController::SetPropLocations()
{
	// Define a minimum distance that props should maintain between each other
	const float MinDistanceBetweenProps = 2000.f;

	// Track which locations have been used
	TArray<USceneComponent*> UsedLocations;

	// Randomly place chosen props while ensuring they are far apart
	for (int32 i = 0; i < ChosenProps.Num(); i++)
	{
		USceneComponent* SelectedLocation = nullptr;

		// Find a valid location for the current chosen prop
		for (USceneComponent* PotentialLocation : ChildrenSpawnLocations)
		{
			bool bIsLocationValid = true;

			// Ensure the location is far enough from already placed props
			for (USceneComponent* UsedLocation : UsedLocations)
			{
				float Distance = FVector::Dist(PotentialLocation->GetComponentLocation(), UsedLocation->GetComponentLocation());

				if (Distance < MinDistanceBetweenProps)
				{
					bIsLocationValid = false;
					break;
				}
			}

			if (bIsLocationValid)
			{
				SelectedLocation = PotentialLocation;
				break;
			}
		}

		// If no valid location is found, fallback to a random remaining location
		if (!SelectedLocation && ChildrenSpawnLocations.Num() > 0)
		{
			int32 RandomIndex = FMath::RandRange(0, ChildrenSpawnLocations.Num() - 1);
			SelectedLocation = ChildrenSpawnLocations[RandomIndex];
			ChildrenSpawnLocations.RemoveAt(RandomIndex);
		}

		// Place the chosen prop at the valid or fallback location
		if (SelectedLocation)
		{
			SetPropLocationAndRotation(ChosenProps[i], SelectedLocation->GetComponentLocation(), SelectedLocation->GetComponentRotation());
			UsedLocations.Add(SelectedLocation);
			ChildrenSpawnLocations.Remove(SelectedLocation);
		}
	}

	// Adjust PropsToSpawn to exclude ChosenProps
	int32 RemainingPropsToSpawn;

	// Catch for if the spawn number is more than the total props
	if (PropsToSpawn >= AllProps.Num())
	{
		RemainingPropsToSpawn = AllProps.Num() - ChosenProps.Num();
	}
	else
	{
		RemainingPropsToSpawn = PropsToSpawn - ChosenProps.Num();
	}

	// Handle remaining props to fill the available slots
	TArray<TObjectPtr<AGOProp>> ExtraProps;
	while (ExtraProps.Num() < RemainingPropsToSpawn)
	{
		AGOProp* Prop = PickProp();

		// Ensure the prop is not already in the chosen or extra props
		if (!ChosenProps.Contains(Prop) && !ExtraProps.Contains(Prop))
		{
			ExtraProps.Add(Prop);
		}
	}

	// Place extra props at available spawn locations
	for (int32 i = 0; i < ChildrenSpawnLocations.Num(); i++)
	{
		if (ExtraProps.Num() > 0)
		{
			USceneComponent* Child = ChildrenSpawnLocations[i];

			// Pick a random prop from the extra props array
			int32 RandomIndex = FMath::RandRange(0, ExtraProps.Num() - 1);
			TObjectPtr<AGOProp> Prop = ExtraProps[RandomIndex];

			// Place the prop at the selected spawn location
			SetPropLocationAndRotation(Prop, Child->GetComponentLocation(), Child->GetComponentRotation());

			// Remove the selected prop from the extra props list
			ExtraProps.RemoveAt(RandomIndex);
		}
	}
}
void AGOPropController::SetPropLocationAndRotation(AGOProp* Prop, FVector ActorLocation, FRotator ActorRotation)
{
	if (!Prop) return;

	// Set the StoredProp location and rotation to the PropToStore's
	Prop->SetActorLocation(ActorLocation);
	Prop->SetActorRotation(ActorRotation);
}

void AGOPropController::StoreInteractedProp(AGOProp* PropToStore)
{
	ResetPropLocation(PropToStore);
}

void AGOPropController::SwapInteractedProps(AGOProp* StoredProp, AGOProp* PropToStore)
{
	if (!PropToStore && !StoredProp) return;

	// Set the PropToStore location and rotation to the stored prop's
	FVector ToStoreLocation = PropToStore->GetActorLocation();
	FRotator ToStoreRotation = PropToStore->GetActorRotation();

	// Set the StoredProp location and rotation to the PropToStore's
	SetPropLocationAndRotation(StoredProp, ToStoreLocation, ToStoreRotation);

	// Set the PropToStore location and rotation to off screen
	ResetPropLocation(PropToStore);
}

bool AGOPropController::IsCorrectProp(AGOProp* Prop, int32 BillboardIndex)
{
	if (!ChosenProps.Contains(Prop)) return false;
	if (ChosenProps.Num() < BillboardIndex + 1) return false;
	if (ChosenProps[BillboardIndex] == Prop) return true;
	return false;
}
void AGOPropController::AddToInaccessibleProps(AGOProp* Prop)
{
	InaccessibleProps.AddUnique(Prop);
	Prop->SetPropCanBePickedUp(false);
}
void AGOPropController::ResetInaccessibleProps()
{
	for (AGOProp* Prop : InaccessibleProps)
	{
		Prop->SetPropCanBePickedUp(true);
	}
	InaccessibleProps.Empty();
}

bool AGOPropController::PropsCollected()
{
	return InaccessibleProps.Num() == PropsToChoose;
}
