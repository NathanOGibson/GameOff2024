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

	/** Handles Prop management */
	UFUNCTION(BlueprintCallable, Category = "PropController")
	void ResetAndInitialiseProps();
	UFUNCTION(BlueprintCallable, Category = "PropController")
	void SetupPropsAndSpawns(TArray<AGOProp*>& ChosenTargetProps);
	UFUNCTION(BlueprintCallable, Category = "PropController")
	void ClearAndResetProps();

private:
	/** Scene components */
	TObjectPtr<USceneComponent> ControllerSceneComponent;

	/** The number of props to choose and spawn */
	int32 PropsToSpawn = 6;
	int32 PropsSavedSpawnCount;
	int32 PropsToChoose = 1;

	/** Arrays for prop references and spawn locations */
	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<TObjectPtr<AGOProp>> AllProps;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<TObjectPtr<AGOProp>> ChosenProps;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<TObjectPtr<AGOProp>> InaccessibleProps;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<USceneComponent*> ChildrenSpawnLocations;

	/** Handle Prop references */
	void InitialisePropReferences();
	TObjectPtr<AGOProp> PickProp();
	void SelectChoseProps();
	void ClearPropData();

	/** Handle selecting spawn positions */
	void InitialiseSpawnLocations();
	USceneComponent* GetFurthestSpawnPoint(TArray<USceneComponent*>& StartPoint, TArray<USceneComponent*>& ChildrenPoints);

	/** Handle Prop positioning */
	void ResetAllPropLocations();
	void ResetPropLocation(AGOProp* Prop);
	void SetPropLocations();

public:
	/** Set Prop location and rotation */
	UFUNCTION(BlueprintCallable, Category = "PropController")
	void SetPropLocationAndRotation(AGOProp* Prop, FVector ActorLocation, FRotator ActorRotation);

	/** Handle storing and swapping props */
	UFUNCTION(BlueprintCallable, Category = "PropController")
	void StoreInteractedProp(AGOProp* PropToStore);
	UFUNCTION(BlueprintCallable, Category = "PropController")
	void SwapInteractedProps(AGOProp* StoredProp, AGOProp* PropToStore);

	/** Check if Prop is the correct ChosenProp using BillboardIndex */
	UFUNCTION(BlueprintCallable, Category = "PropController")
	bool IsCorrectProp(AGOProp* Prop, int32 BillboardIndex);

	/** Handle inaccessible Props list */
	UFUNCTION(BlueprintCallable, Category = "PropController")
	void AddToInaccessibleProps(AGOProp* Prop);
	UFUNCTION(BlueprintCallable, Category = "PropController")
	void ResetInaccessibleProps();

	/** Return true if InaccessibleProps amount equals ChosenProps */
	UFUNCTION(BlueprintCallable, Category = "PropController")
	bool PropsCollected();

	/** Assign amount of Props to choose and spawn */
	UFUNCTION(BlueprintCallable, Category = "PropController")
	FORCEINLINE void SetPropsToChooseAndSpawn(int32 NewPropsToChoose, int32 NewPropsToSpawn) { PropsToChoose = NewPropsToChoose; PropsToSpawn = NewPropsToSpawn; }









};
