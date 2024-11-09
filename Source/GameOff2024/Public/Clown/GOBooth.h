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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Booth")
	TObjectPtr<AGOPropController> PropController;

	UFUNCTION(BlueprintCallable, Category = "Booth")
	void InitialiseReferences();

protected:
	virtual void BeginPlay() override;

private:


public:
	virtual void Tick(float DeltaTime) override;

};
