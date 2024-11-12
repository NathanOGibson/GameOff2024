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

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	FString PropName = "";

protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<USceneComponent> PropSceneComponent;

	UPROPERTY(EditAnywhere, Category = "Prop")
	TObjectPtr<UStaticMeshComponent> PropMesh;

	UPROPERTY(VisibleAnywhere, Category = "Prop")
	bool bIsPickedUp = false;



public:
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE FString GetPropName() const { return PropName; }
	FORCEINLINE bool GetIsPickedUp() const { return bIsPickedUp; }
};
