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

	UFUNCTION(BlueprintCallable, Category = "PropController")
	void ResetAndInitialiseProps();

	UFUNCTION(BlueprintCallable, Category = "PropController")
	void SetupPropsAndSpawns(TArray<AGOProp*>& ChosenTargetProps);

	UFUNCTION(BlueprintCallable, Category = "PropController")
	void ClearAndResetProps();

private:
	TObjectPtr<USceneComponent> ControllerSceneComponent;

	int32 PropsToSpawn = 6;
	int32 PropsToChoose = 1;

	int32 PropsSavedSpawnCount;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<TObjectPtr<AGOProp>> AllProps;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<TObjectPtr<AGOProp>> ChosenProps;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<TObjectPtr<AGOProp>> InaccessibleProps;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<USceneComponent*> ChildrenSpawnLocations;

	/** Assigning Props*/
	void InitialisePropReferences();

	TObjectPtr<AGOProp> PickProp();

	void SelectChoseProps();

	void ClearPropData();

	/** Positioning Props*/
	void ResetPropLocations();

	void InitialiseSpawnLocations();

	USceneComponent* GetFurthestSpawnPoint(TArray<USceneComponent*>& StartPoint, TArray<USceneComponent*>& ChildrenPoints);

	void SetPropLocations();


	void ResetPropLocation(AGOProp* Prop);



public:
	UFUNCTION(BlueprintCallable, Category = "PropController")
	void StoreInteractedProp(AGOProp* PropToStore);

	UFUNCTION(BlueprintCallable, Category = "PropController")
	void SwapInteractedProps(AGOProp* StoredProp, AGOProp* PropToStore);

	UFUNCTION(BlueprintCallable, Category = "PropController")
	void SetPropLocationAndRotation(AGOProp* Prop, FVector ActorLocation, FRotator ActorRotation);

	UFUNCTION(BlueprintCallable, Category = "PropController")
	FORCEINLINE void SetPropsToChooseAndSpawn(int32 NewPropsToChoose, int32 NewPropsToSpawn) { PropsToChoose = NewPropsToChoose; PropsToSpawn = NewPropsToSpawn; }

	UFUNCTION(BlueprintCallable, Category = "PropController")
	FORCEINLINE bool IsCorrectProp(AGOProp* Prop, int32 BillboardIndex);

	UFUNCTION(BlueprintCallable, Category = "PropController")
	void AddToInaccessibleProps(AGOProp* Prop);

	UFUNCTION(BlueprintCallable, Category = "PropController")
	void ResetInaccessibleProps();

	UFUNCTION(BlueprintCallable, Category = "PropController")
	bool PropsCollected();

};
