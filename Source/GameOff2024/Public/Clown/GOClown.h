// Copyright Nah Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GOClown.generated.h"

UCLASS()
class GAMEOFF2024_API AGOClown : public ACharacter
{
	GENERATED_BODY()

public:
	AGOClown();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
