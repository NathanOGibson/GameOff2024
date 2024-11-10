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
	void SetupPropsAndSpawns(AGOProp*& ChosenTargetProp, AGOProp*& FirstBackupProp, AGOProp*& SecondBackupProp);

	UFUNCTION(BlueprintCallable, Category = "PropController")
	void ClearAndResetProps();

private:
	TObjectPtr<USceneComponent> ControllerSceneComponent;

	UPROPERTY(EditAnywhere, Category = "Prop")
	int32 PropsToSpawn = 6;

	int32 PropsSavedSpawnCount;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<TObjectPtr<AGOProp>> AllProps;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TObjectPtr<AGOProp> ChosenProp;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<TObjectPtr<AGOProp>> BackupProps;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	TArray<USceneComponent*> ChildrenSpawnLocations;

	/** Assigning Props*/
	void InitialisePropReferences();

	TObjectPtr<AGOProp> PickProp();

	void SelectPrimaryAndBackupProps();

	void ClearPropData();

	/** Positioning Props*/
	void ResetPropLocations();

	void InitialiseSpawnLocations();

	USceneComponent* GetFurthestSpawnPoint(TArray<USceneComponent*>& StartPoint, TArray<USceneComponent*>& ChildrenPoints);

	void SetPropLocations();


public:
	virtual void Tick(float DeltaTime) override;

};
