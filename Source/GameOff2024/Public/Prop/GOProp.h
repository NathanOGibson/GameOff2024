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

	/** Prop variables */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	FString PropName = "";
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Prop")
	TObjectPtr<UTexture2D> PropImage;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prop")
	bool PropCanBePickedUp = true;

protected:
	virtual void BeginPlay() override;

	/** Prop components */
	TObjectPtr<USceneComponent> PropSceneComponent;
	UPROPERTY(EditAnywhere, Category = "Prop")
	TObjectPtr<UStaticMeshComponent> PropMesh;

public:

	/** Return PropName */
	FORCEINLINE FString GetPropName() const { return PropName; }

	/** Return PropCanBePickedUp */
	UFUNCTION(BlueprintCallable, Category = "Prop")
	FORCEINLINE bool GetPropCanBePickedUp() const { return PropCanBePickedUp; }

	/** Set PropCanBePickedUp */
	UFUNCTION(BlueprintCallable, Category = "Prop")
	FORCEINLINE void  SetPropCanBePickedUp(bool CanBePickedUp) { PropCanBePickedUp = CanBePickedUp; }
};
