// Copyright Nah Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GOBooth.generated.h"

class AGOPropController;

UCLASS()
class GAMEOFF2024_API AGOBooth : public AActor
{
	GENERATED_BODY()

public:
	AGOBooth();

	/** PropController reference */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop Controller")
	TObjectPtr<AGOPropController> PropController;

	/** Billboard array reference */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Sign Billboards")
	TArray<TObjectPtr<UBillboardComponent>> TicketBillboards;

	/** Initialise Prop Controller */
	UFUNCTION(BlueprintCallable, Category = "Prop Controller")
	void InitialiseReferences();

protected:
	virtual void BeginPlay() override;

private:
	/** Booth components */
	TObjectPtr<USceneComponent> BoothSceneComponent;
	UPROPERTY(EditAnywhere, Category = "Mesh")
	TObjectPtr<UStaticMeshComponent> BoothMesh;
	UPROPERTY(EditAnywhere, Category = "Mesh")
	TArray<TObjectPtr<UStaticMeshComponent>> TicketMeshes;

	/** Setup ticket and billboard components */
	void CreateTicketMeshesAndBillboards();

public:
	virtual void Tick(float DeltaTime) override;

	/** Returns TicketMeshes */
	UFUNCTION(BlueprintCallable, Category = "Ticket")
	FORCEINLINE TArray<UStaticMeshComponent*> GetTicketMeshes() const { return TicketMeshes; }
};
