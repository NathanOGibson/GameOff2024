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

protected:
	/** PatrolPoint components */
	TObjectPtr<USceneComponent> PatrolPointSceneComponent;
	UPROPERTY(EditAnywhere, Category = "PatrolPoint")
	TObjectPtr<UStaticMeshComponent> PatrolPointMesh;

};
