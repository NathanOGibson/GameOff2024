// Copyright Nah Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GOPropController.generated.h"

class AGOProp;

UCLASS()
class GAMEOFF2024_API AGOPropController : public AActor
{
	GENERATED_BODY()

public:
	AGOPropController();

protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<USceneComponent> ControllerSceneComponent;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<TObjectPtr<AGOProp>> AllProps;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TObjectPtr<AGOProp> ChosenProp;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<TObjectPtr<AGOProp>> ExtraProps;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<USceneComponent*> ChildrenSpawnLocations;

	/** Selecting Props*/
	void GetPropReferences();

	TObjectPtr<AGOProp> PickProp();

	void AssignChosenAndExtraProps();

	/** Positioning Props*/
	void ResetPropPositions();

	void SetSpawnPoints();

	USceneComponent* GetFurthestSpawnPoint(USceneComponent* StartPoint, TArray<USceneComponent*> ChildrenPoints);

	void SetPropPositions();


public:
	virtual void Tick(float DeltaTime) override;

};
