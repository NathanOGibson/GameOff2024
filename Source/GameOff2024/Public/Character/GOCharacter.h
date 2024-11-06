// Copyright Nah Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GOCharacter.generated.h"

UCLASS()
class GAMEOFF2024_API AGOCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGOCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
