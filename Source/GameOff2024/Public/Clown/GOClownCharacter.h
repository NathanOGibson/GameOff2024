// Copyright Nah Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GOClownCharacter.generated.h"

UENUM(BlueprintType)
enum class EClownState : uint8
{
	ECS_Idle UMETA(DisplayName = "Idle"),
	ECS_GetPatrolPoint UMETA(DisplayName = "GetPatrolPoint"),
	ECS_Patrol UMETA(DisplayName = "Patrot"),
	ECS_Chase UMETA(DisplayName = "Chase"),
	ECS_Jumpscare UMETA(DisplayName = "Jumpscare"),
};

class AGOClownAIController;

UCLASS()
class GAMEOFF2024_API AGOClownCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGOClownCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Function for jumpscare, called when close to player
	UFUNCTION(BlueprintCallable, Category = "Clown Behavior")
	void JumpscarePlayer();

private:
	AGOClownAIController* ClownAIController;

	AActor* Player;

	// AI States
	UPROPERTY(VisibleAnywhere, Category = "Clown Behavior")
	EClownState ClownState;

	// Patrol radius for the clown
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float PatrolRadius = 1000.f;

	// Detection range for the player
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float DetectionRange = 1500.f;

	// Jumpscare range
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float JumpscareRange = 200.f;

};
