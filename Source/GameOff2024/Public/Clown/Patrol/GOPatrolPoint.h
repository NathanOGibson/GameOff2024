// Copyright Nah Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GOPatrolPoint.generated.h"

UCLASS()
class GAMEOFF2024_API AGOPatrolPoint : public AActor
{
	GENERATED_BODY()

public:
	AGOPatrolPoint();

	UPROPERTY(EditAnywhere, Category = "PatrolPoint")
	TObjectPtr<UMaterial> ActiveMaterial;
	UPROPERTY(EditAnywhere, Category = "PatrolPoint")
	TObjectPtr<UMaterial> DisabledMaterial;
	UPROPERTY(EditAnywhere, Category = "PatrolPoint")
	TObjectPtr<UMaterial> CachedMaterial;

	void SetToMaterialActive();
	void SetMaterialToDisabled();
	void SetMaterialToCached();


protected:
	/** PatrolPoint components */
	TObjectPtr<USceneComponent> PatrolPointSceneComponent;
	UPROPERTY(EditAnywhere, Category = "PatrolPoint")
	TObjectPtr<UStaticMeshComponent> PatrolPointMesh;

	void SetMaterial(TObjectPtr<UMaterial> NewMaterial);

};
