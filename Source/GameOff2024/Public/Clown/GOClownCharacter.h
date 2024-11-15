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
	ECS_GetSearchPoint UMETA(DisplayName = "GetSearchPoint"),
	ECS_Search UMETA(DisplayName = "Search"),
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

	//UFUNCTION(BlueprintImplementableEvent, Category = "ClownState")
	//void DelayEvent(float DelayAmount);


	UFUNCTION(BlueprintImplementableEvent, Category = "ClownState")
	void IdleDelay(float DelayAmount);

	UFUNCTION(BlueprintImplementableEvent, Category = "ClownState")
	void PatrolDelay(float DelayAmount);

	UFUNCTION(BlueprintImplementableEvent, Category = "ClownState")
	void ChaseDelay(float DelayAmount);


	UFUNCTION(BlueprintImplementableEvent, Category = "ClownState")
	void SearchDelay(float DelayAmount);

	//////////////////////
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Movement")
	bool CheckPlayerIsMoving();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Movement")
	bool CheckPlayerIsCrouching();

	UFUNCTION(BlueprintImplementableEvent, Category = "Player Movement")
	bool CheckPlayerIsSprinting();

	//////////////////////

	UPROPERTY(EditAnywhere, Category = "ClownState")
	float IdleDelayAmount = 5.f;

	UPROPERTY(EditAnywhere, Category = "ClownState")
	float PatrolDelayAmount = 1.f;

	UPROPERTY(EditAnywhere, Category = "ClownState")
	float ChaseDelayAmount = 5.f;

	UPROPERTY(EditAnywhere, Category = "ClownState")
	float SearchDelayAmount = 8.f;




private:
	AGOClownAIController* ClownAIController;

	AActor* Player;

	// AI States
	UPROPERTY(VisibleAnywhere, Category = "Clown Behavior")
	EClownState ClownState;

	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float PatrolDistance = 1000.f;

	// Patrol radius for the clown
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float PatrolRadius = 1000.f;

	// Detection range for the player
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float DetectionQueryRange = 1500.f;

	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float DetectionAgroRange = 500.f;

	// Jumpscare range
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float JumpscareRange = 200.f;

	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float DetectionAngle = 90.f;

	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float PatrolMovementSpeed = 200.f;

	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float ChaseMovementSpeed = 400.f;

	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float SearchMovementSpeed = 300.f;

	void SetCharacterSpeed(float Speed);

	/////////////////////////////////////////////////////////////////////////

	/* Detection Functions **/
	bool CheckPlayerWithinDetectionRange();
	void AdjustClownDetection(float DetectionAdjustmentAmount);
	void DebugDetectionRange();
	bool IsPlayerWithinDetectionRange(float DistanceToPlayer);
	bool IsPlayerWithinDetectionAngle(FVector PlayerLocation);
	bool IsPlayerWithinAgroRange(float DistanceToPlayer);
	bool IsPlayerInLineOfSight(FVector PlayerLocation, FVector ClownLocation);
	void AdjustClownDetectionBasedOnPlayerMovement();

	/////////////////////////////////////////////////////////////////////////

	void TempFunc();

	UPROPERTY(VisibleAnywhere, Category = "Clown Detection")
	float ClownDetection;
	UPROPERTY(EditAnywhere, Category = "Clown Detection")
	float DetectionThreshold = 30.f;
	UPROPERTY(EditAnywhere, Category = "Clown Detection")
	float DetectionDecayRate = 1.f;

	UPROPERTY(EditAnywhere, Category = "Clown Detection")
	float DetectionNotWalkingIncreaseRate = 2.f;

	UPROPERTY(EditAnywhere, Category = "Clown Detection")
	float DetectionWalkingIncreaseRate = 3.f;

	UPROPERTY(EditAnywhere, Category = "Clown Detection")
	float DetectionCrouchingIncreaseRate = 3.f;

	UPROPERTY(EditAnywhere, Category = "Clown Detection")
	float DetectionSprintingIncreaseRate = 4.f;

	//////////////////////


	//////////////////////

	/** Idle state function */
	void HandleIdleState();

	/** GetPatrolPoint state function */
	void HandleGetPatrolPointState();

	/** Patrol state function */
	void HandlePatrolState();

	/** Chase state function */
	void HandleChaseState();

	/** GetSearchPoint state function */
	void HandlGetSearchPointState();

	/** Search state function */
	void HandleSearchState();

	/** Jumpscare state function */
	void HandleJumpscareState();


public:
	UFUNCTION(BlueprintCallable, Category = "ClownState")
	FORCEINLINE void SetClownState(EClownState NewClownState) { ClownState = NewClownState; }

	UFUNCTION(BlueprintCallable, Category = "Player Movement")
	FORCEINLINE AActor* GetPlayer() { return Player; }
};
