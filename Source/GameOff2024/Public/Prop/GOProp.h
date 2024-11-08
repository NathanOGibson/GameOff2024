// Copyright Nah Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GOProp.generated.h"

UCLASS()
class GAMEOFF2024_API AGOProp : public AActor
{
	GENERATED_BODY()

public:
	AGOProp();

protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<USceneComponent> PropSceneComponent;

	UPROPERTY(EditAnywhere, Category = "Prop")
	TObjectPtr<UStaticMeshComponent> PropMesh;

	UPROPERTY(EditAnywhere, Category = "Prop")
	FString PropName = "";

public:
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE FString GetPropName() const { return PropName; }
};
