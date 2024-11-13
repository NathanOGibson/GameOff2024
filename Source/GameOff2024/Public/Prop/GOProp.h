// Copyright Nah Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GOProp.generated.h"

class UTexture2D;

UCLASS()
class GAMEOFF2024_API AGOProp : public AActor
{
	GENERATED_BODY()

public:
	AGOProp();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	FString PropName = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	TObjectPtr<UTexture2D> PropImage;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop")
	bool PropCanBePickedUp = true;

protected:
	virtual void BeginPlay() override;

private:
	TObjectPtr<USceneComponent> PropSceneComponent;

	UPROPERTY(EditAnywhere, Category = "Prop")
	TObjectPtr<UStaticMeshComponent> PropMesh;



public:
	virtual void Tick(float DeltaTime) override;

	FORCEINLINE FString GetPropName() const { return PropName; }

	UFUNCTION(BlueprintCallable, Category = "Prop")
	FORCEINLINE bool GetPropCanBePickedUp() const { return PropCanBePickedUp; }

	UFUNCTION(BlueprintCallable, Category = "Prop")
	FORCEINLINE void  SetPropCanBePickedUp(bool CanBePickedUp) { PropCanBePickedUp = CanBePickedUp; }
};
