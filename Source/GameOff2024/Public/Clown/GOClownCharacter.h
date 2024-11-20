// Copyright Nah Studios

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GOClownCharacter.generated.h"

UENUM(BlueprintType)
enum class EClownState : uint8
{
	ECS_Idle UMETA(DisplayName = "Idle"),
	ECS_GetPatrolPoint UMETA(DisplayName = "Get Patrol Point"),
	ECS_Patrol UMETA(DisplayName = "Patrot"),
	ECS_Chase UMETA(DisplayName = "Chase"),
	ECS_Activated UMETA(DisplayName = "Activeated"),
	ECS_GetSearchPoint UMETA(DisplayName = "Get Search Point"),
	ECS_Search UMETA(DisplayName = "Search"),
	ECS_Retreat UMETA(DisplayName = "Retreat"),
	ECS_Jumpscare UMETA(DisplayName = "Jumpscare"),
};

class AGOClownAIController;
class UBillboardComponent;

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

	/* Delay functions implemented in blueprints **/
	UFUNCTION(BlueprintImplementableEvent, Category = "Clown Delay")
	void IdleDelay(float DelayAmount);
	UFUNCTION(BlueprintImplementableEvent, Category = "Clown Delay")
	void PatrolDelay(float DelayAmount);
	UFUNCTION(BlueprintImplementableEvent, Category = "Clown Delay")
	void ChaseDelay(float DelayAmount);
	UFUNCTION(BlueprintImplementableEvent, Category = "Clown Delay")
	void ActiveatedDelay(float DelayAmount);
	UFUNCTION(BlueprintImplementableEvent, Category = "Clown Delay")
	void SearchDelay(float DelayAmount);

	/* Player movement checks implemented in blueprints **/
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Movement")
	bool CheckPlayerIsMoving();
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Movement")
	bool CheckPlayerIsCrouching();
	UFUNCTION(BlueprintImplementableEvent, Category = "Player Movement")
	bool CheckPlayerIsSprinting();

	/* Jumpscare functionality in blueprints **/
	UFUNCTION(BlueprintImplementableEvent, Category = "Clown Jumpscare")
	void JumpscarePlayer();

private:
	/** AI Controller for the clown */
	AGOClownAIController* ClownAIController;

	/** Player reference */
	AActor* Player;

	/** Is the clown active? */
	UPROPERTY(VisibleAnywhere, Category = "Clown Behavior")
	bool bAIActive = false;

	/** Current AI state of the clown */
	UPROPERTY(VisibleAnywhere, Category = "Clown Behavior")
	EClownState ClownState;

	/** Delay values for different states */
	UPROPERTY(EditAnywhere, Category = "Clown Delay")
	float IdleDelayAmount = 5.f;
	UPROPERTY(EditAnywhere, Category = "Clown Delay")
	float PatrolDelayAmount = 1.f;
	UPROPERTY(EditAnywhere, Category = "Clown Delay")
	float ChaseDelayAmount = 5.f;
	UPROPERTY(EditAnywhere, Category = "Clown Delay")
	float ActiveatedDelayAmount = 10.f;
	UPROPERTY(EditAnywhere, Category = "Clown Delay")
	float SearchDelayAmount = 8.f;

	/** Detection state properties */
	UPROPERTY(VisibleAnywhere, Category = "Clown Detection")
	float ClownDetection;

	/** Detection thresholds and rates */
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

	/** Detection ranges, angles, and speeds */
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float PatrolDistance = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float CachedDistance = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float PatrolRadius = 1000.f;
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float DetectionQueryRange = 1500.f;
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float DetectionAgroRange = 500.f;
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float JumpscareRange = 200.f;
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float DetectionAngle = 90.f;
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float PatrolMovementSpeed = 200.f;
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float ChaseMovementSpeed = 400.f;
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float ActiveatedMovementSpeed = 400.f;
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float SearchMovementSpeed = 300.f;
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float RetreatMovementSpeed = 1400.f;

	/** Distance threshold */
	UPROPERTY(EditAnywhere, Category = "Clown Behavior")
	float DistanceThreshold = 100.f;

	/* Set AI controller inactive/active **/
	UFUNCTION(BlueprintCallable, Category = "Clown AI Controller")
	void SetControllerInactive();
	UFUNCTION(BlueprintCallable, Category = "Clown AI Controller")
	void SetControllerActive(FVector NewCharacterLocation, FRotator NewCharacterRotation);

	/** Adjust character speed functions */
	void SetCharacterSpeed(float Speed);

	/** Detection functions */
	bool CheckPlayerWithinDetectionRange();
	void AdjustClownDetection(float DetectionAdjustmentAmount);
	void DebugDetectionRange();
	bool IsPlayerWithinDetectionRange(float DistanceToPlayer);
	bool IsPlayerWithinDetectionAngle(FVector PlayerLocation);
	bool IsPlayerWithinAgroRange(float DistanceToPlayer);
	bool IsPlayerInLineOfSight(FVector PlayerLocation, FVector ClownLocation);
	void AdjustClownDetectionBasedOnPlayerMovement();
	bool IsPlayerWithinJumpscareRange();

	/** Set to retreat state */
	UFUNCTION(BlueprintCallable, Category = "Clown Retreat")
	void SetToRetreatState(FVector NewRetreatLocation);
	UFUNCTION(BlueprintCallable, Category = "Clown Retreat")
	bool HasFinishedRetreating();

	/** Stored location checks */
	FVector PatrolPoint;
	FVector SearchPoint;
	FVector RetreatPoint;

	/** State functions */
	void HandleIdleState();
	void HandleGetPatrolPointState();
	void HandlePatrolState();
	void HandleChaseState();
	void HandleActiveatedState();
	void HandlGetSearchPointState();
	void HandleSearchState();
	void HandleRetreatState();
	void HandleJumpscareState();

public:
	/** Set the AI's state */
	UFUNCTION(BlueprintCallable, Category = "ClownState")
	FORCEINLINE void SetClownState(EClownState NewClownState) { ClownState = NewClownState; }

	/** Get the AI's state */
	UFUNCTION(BlueprintCallable, Category = "ClownState")
	FORCEINLINE EClownState GetClownState() { return ClownState; }

	/** Get the player reference */
	UFUNCTION(BlueprintCallable, Category = "Player Movement")
	FORCEINLINE AActor* GetPlayer() { return Player; }
};
