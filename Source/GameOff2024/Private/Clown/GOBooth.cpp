// Copyright Nah Studios


#include "Clown/GOBooth.h"
#include "Prop/GOPropController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BillboardComponent.h"

AGOBooth::AGOBooth()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create the root scene component
	BoothSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BoothSceneComponent"));
	BoothSceneComponent->SetupAttachment(RootComponent);
	SetRootComponent(BoothSceneComponent);

	// Create the booth mesh
	BoothMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoothMesh"));
	BoothMesh->SetupAttachment(RootComponent);

	// Create the sign meshes and billboards
	for (int32 i = 0; i < 5; i++)
	{
		// Dynamically create static mesh components
		FName SignMeshFName(*FString::Printf(TEXT("SignMesh_%d"), i + 1));
		UStaticMeshComponent* SignMesh = CreateDefaultSubobject<UStaticMeshComponent>(SignMeshFName);
		if (SignMesh)
		{
			SignMesh->SetupAttachment(RootComponent); // Attach to root
			SignMeshes.Add(SignMesh); // Add to array
		}

		// Dynamically create billboard components and attach to the mesh
		FName SignBillboardName(*FString::Printf(TEXT("SignBillboard_%d"), i + 1));
		UBillboardComponent* SignBillboard = CreateDefaultSubobject<UBillboardComponent>(SignBillboardName);
		if (SignBillboard)
		{
			SignBillboard->SetupAttachment(SignMesh); // Attach to static mesh
			SignBillboards.Add(SignBillboard); // Add to array
		}
	}
}

void AGOBooth::BeginPlay()
{
	Super::BeginPlay();
}

void AGOBooth::InitialiseReferences()
{
	PropController = Cast<AGOPropController>(UGameplayStatics::GetActorOfClass(GetWorld(), AGOPropController::StaticClass()));
}

void AGOBooth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

