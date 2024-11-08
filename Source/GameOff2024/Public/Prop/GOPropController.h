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

	void GetPropReferences();

	TObjectPtr<AGOProp> PickProp();
	void AssignChosenAndExtraProps();

public:
	virtual void Tick(float DeltaTime) override;

};
