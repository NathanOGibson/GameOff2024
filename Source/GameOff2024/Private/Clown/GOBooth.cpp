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

	CreateTicketMeshesAndBillboards();
}

void AGOBooth::CreateTicketMeshesAndBillboards()
{
	// Create the sign meshes and billboards
	for (int32 i = 0; i < 5; i++)
	{
		// Dynamically create static mesh components
		FName TicketMeshFName(*FString::Printf(TEXT("TicketMesh_%d"), i + 1));
		UStaticMeshComponent* TicketMesh = CreateDefaultSubobject<UStaticMeshComponent>(TicketMeshFName);
		if (TicketMesh)
		{
			TicketMesh->SetupAttachment(RootComponent); // Attach to root
			TicketMeshes.Add(TicketMesh); // Add to array
		}

		// Dynamically create billboard components and attach to the mesh
		FName TicketBillboardName(*FString::Printf(TEXT("TicketBillboard_%d"), i + 1));
		UBillboardComponent* TicketBillboard = CreateDefaultSubobject<UBillboardComponent>(TicketBillboardName);
		if (TicketBillboard)
		{
			TicketBillboard->SetupAttachment(TicketMesh); // Attach to static mesh
			TicketBillboards.Add(TicketBillboard); // Add to array
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

