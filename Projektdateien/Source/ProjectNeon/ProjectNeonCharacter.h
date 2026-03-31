// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Containers/Array.h"
#include "ProjectNeonGameMode.h"
#include "ProjectNeonCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTrickPerformedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDashPerformedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnJumpPerformedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCollisionDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStopHoveringDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStartHoveringDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBouncyWallCollisionDelegate);

// ----- Constants ----- //

#define NUM_TRACES 7
#define NUM_LAST_ROTATIONS 20

/**
 * @brief Main character class.
 * 
 * This is the main class for a players character.
 * This class represents a character on a hover board. The character is 
 * constantly hovering over the ground and can perform several
 * movement abilities.
 */
UCLASS(config=Game)
class AProjectNeonCharacter : public ACharacter
{
	GENERATED_BODY()

	/**
	 * Structure for the results of a ground trace.
	 * The results are stored from the front of
	 * the hoverboard to the back. The array of
	 * hit points only contains a valid entry if the 
	 * matching bHits entry is true.
	 */
	struct ListTraceResult {
		TArray<bool> bHits;
		TArray<FVector> HitPoints;
	};
	typedef struct ListTraceResult ListTraceResult;

	/**
	 * Structure for a hitbox trace.
	 * Stores if something has been hit,
	 * the hit point and the hit actor.
	 * The trace is performed from the 
	 * hoverboard center to the front and
	 * to the back.
	 */
	struct DoubleTraceResult {
		bool bFrontHit;
		bool bBackHit;

		FVector FrontHitPoint;
		FVector BackHitPoint;

		AActor* FrontHitActor;
		AActor* BackHitActor;
	};
	typedef struct DoubleTraceResult DoubleTraceResult;

    /** Game Mode Instance */
    AProjectNeonGameMode* GameMode;
	
	// ----- Input ----- //

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Dash Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CrouchAction;

	/** Trick Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* TrickAction;

	// ----- Movement Parameters ----- //

	/** Time when starting to move */
	UPROPERTY(EditAnywhere)
	double StartTime = 0;

	/** Logarithmic time when moving */
	UPROPERTY(EditAnywhere)
	float LogTime = 0;

	/** Strength of Dash */
	UPROPERTY(EditAnywhere)
	float DashStrength = 4000.0f;

	/** Strength of boost by trick */
	UPROPERTY(EditAnywhere)
	float TrickStrength = 700.0f;

	/** Count of possible Jumps in the Air */
	UPROPERTY(EditAnywhere)
	int MaxAerialJumps = 1;

	/** Radius of hitbox */
	UPROPERTY(EditAnywhere)
	float CapsuleRadius = 42.0f;

	/** Half height of hitbox */
	UPROPERTY(EditAnywhere)
	float CapsuleHalfHeight = 96.0f;

	/** Cooldown of dash */
	UPROPERTY(EditAnywhere)
	float DashTimer = 3.0f;

	// ----- Abilities ----- //

	/** Has Player made Jump Input? */
	bool bShouldJump = false;

	/** Has Player made Dash Input? */
	bool bShouldDash = false;

	/* Has player made trick input? */
	bool bShouldTrick = false;

	/** Can Player currently Jump? */
	bool bCanJump = true;

	/** Can Player currently Dash? */
	bool bCanDash = true;

	/** Currently possible aerial jumps */
	int AerialJumpCounter = MaxAerialJumps;

	/** Timer Handle for Jump Cooldown */
	FTimerHandle JumpTimerHandle;

	/** Timer Handlde for Dash Cooldown */
	FTimerHandle DashTimerHandle;

	/** Timer handle for trick time window */
	FTimerHandle TrickTimerHandle;

	/** Timer handle for trick time window */
	FTimerHandle TrickStateTimerHandle;

	/** Timer Handle for Speed Growth */
	FTimerHandle SpeedTimerHandle;

	/** Timer handle for window after hovering stopped*/
	FTimerHandle JumpWindowTimerHandle;

	/** speed a which the character is moving up and down */
	UPROPERTY(EditAnywhere)
	float BobbingSpeed = 1.0f;

	/** distance by which the character is moving up and down */
	UPROPERTY(EditAnywhere)
	float BobbingDistance = 1.0f;

	/** starting location used as reference for bobbing */
	FVector ReferenceLocation;

	/** Is Player moving? */
	bool bIsMoving = false;

	/** Has level timer been started? */
	bool bIsTimerStarted = false;

	/** Is Player hovering? */
	bool bIsHovering;

public:

	/** Base Height for Hovering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HoverHeight = 70.0f;

	/** Base Height for Hovering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HoverBoardFront = 155.0f;

	/** Base Height for Hovering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HoverBoardBack = 210.0f;

	/** Base Height for Hovering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float HoverBoardTop = 10.0f;

	/** Array of last rotation of character for interpolation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> LastRotation;

	/** Base Height for Hovering */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CrouchingHitbox;

	/** Reference to hitbox of character */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UCapsuleComponent* StandingHitbox;

	UPROPERTY(BlueprintAssignable)
	FOnTrickPerformedDelegate OnTrickPerformed;

	UPROPERTY(BlueprintAssignable)
	FOnDashPerformedDelegate OnDashPerformed;

	UPROPERTY(BlueprintAssignable)
	FOnJumpPerformedDelegate OnJumpPerformed;

	UPROPERTY(BlueprintAssignable)
	FOnCollisionDelegate OnCollision;

	UPROPERTY(BlueprintAssignable)
	FOnStopHoveringDelegate OnStopHovering;

	UPROPERTY(BlueprintAssignable)
	FOnStartHoveringDelegate OnStartHovering;

	UPROPERTY(BlueprintAssignable)
	FOnBouncyWallCollisionDelegate OnBouncyWallCollision;

	/**
	 * Construct a new AProjectNeonCharacter object.
	 * 
	 * Sets up the ticks for this character. Calls the setup of
	 * movement parameters and initializes the collision component size.
	 * 
	 * @param ObjectInitializer initializer
	 */
	AProjectNeonCharacter(const FObjectInitializer& ObjectInitializer);

	// ----- Cooldowns & Timers ----- //

	/**
	 * Returns the remaining cooldown 
	 * the dash ability.
	 * 
	 * @return float cooldown in seconds
	 */
	UFUNCTION(BlueprintCallable)
	float GetDashCooldown();

	/**
	 * Returns the maximum
	 * cooldown of the dash ability.
	 * 
	 * @return max cooldown in seconds
	 */
	UFUNCTION(BlueprintCallable)
	float GetDashTimer();

	/**
	 * Returns if the trick window
	 * is currently active.
	 * 
	 * @return true trick window is active
	 */
	UFUNCTION(BlueprintCallable)
	bool GetTrickWindow();

	// ----- Level Timer ----- //

	/**
	 * Starts the timer for the
	 * current level.
	 */
	UFUNCTION(BlueprintCallable)
	void StartTimer();

	/**
	 * Ends the timer for the
	 * current level.
	 */
	UFUNCTION(BlueprintCallable)
	void StopTimer(int LevelIdx);

	/**
	 * Returns the speed at which the
	 * character is moving along the
	 * x-axis.
	 * 
	 * @return float speed in cm/s
	 */
	UFUNCTION(BlueprintCallable)
	float GetSpeed();

	UFUNCTION(BlueprintCallable)
	bool GetMakingTrick();

protected:

	/**
	 * @brief Setup of movement parameters.
	 * 
	 * Sets up all parameters for this characters movement.
	 * 
	 */
	void SetupCharacterMovement();

	/**
	 * Gets the current speed of the character.
	 *
	 * @return The current speed of the character.
	 */
	float GetCurrentSpeed();
	
	/**
	 * @brief Handles the setup of the InputComponent.
	 * 
	 * Assigns all Methods to their corresponding inputs.
	 * 
	 * @param PlayerInputComponent InputComponent of the player
	 */
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	/**
	 * @brief Called on spawn.
	 * 
	 * Initializes the players input mapping context.
	 */
	virtual void BeginPlay();

	// ========== Hovering ========== //

	/**
	 * Makes the character start hovering.
	 */
	void StartHover(void);

	/**
	 * Ends the hovering of the character.
	 */
	void StopHover(void);

	/**
	 * Performs a trace from the hoverboard to the ground.
	 * 
	 * @return ListTraceResult result of trace
	 */
	ListTraceResult TraceGround(void);

	/**
	 * Performs a trace upwards for hoverboard
	 * collision.
	 * 
	 * @return ListTraceResult result of trace 
	 */
	ListTraceResult TraceSky(void);

	/**
	 * Traces forward and backward for hitbox
	 * of hoverboard.
	 * 
	 * @return DoubleTraceResult result of trace 
	 */
	DoubleTraceResult TraceHitbox(void);

	/**
	 * Traces to all four sides to chaeck collision with bouncewalls.
	 * 
	 * @return DoubleTraceResult results of character hitbox tracing
	 */
	DoubleTraceResult TraceCharHitbox(void);

	/**
	 * Performs a trace for determining
	 * the rotation of the character around
	 * the y-axis.
	 * 
	 * @return DoubleTraceResult result of trace 
	 */
	DoubleTraceResult TraceForRotation(void);

	/**
	 * Checks for a collision and applies
	 * the appropriate reaction.
	 */
	void CheckCollision(void);

	/**
	 * Checks if the given trace contains a
	 * valid hit.
	 * 
	 * @param Trace trace to check 
	 * @return true if something has been hit
	 * @return false if nothing has been hit
	 */
	bool CheckHit(ListTraceResult Trace);

	/**
	 * Retrieves the height of the highest hit point
	 * of the given trace.
	 * 
	 * @param Trace trace to check
	 * @return float height of highest hit point
	 */
	float GetMaxGround(ListTraceResult Trace);

	/**
	 * Updates the list of the last rotations.
	 * The rotation is added to the front of the list
	 * and pushes the oldest rotation out.
	 * 
	 * @param NewRotation new rotation value
	 */
	void UpdateLastRotation(float NewRotation);

	/**
	 * Returns the sum of the last rotation
	 * list.
	 * 
	 * @return float sum of all last rotations 
	 */
	float SumOfLastRotation(void);

	/**
	 * Initializes the list of last rotations.
	 * 
	 * @param initVal value to initialize with
	 */
	void InitLastRotation(float initVal);

	/**
	 * @brief Handles the hovering mechanism
	 * 
	 * This method should be called every tick to controll
	 * the hovering mechanism of the character.
	 * 
	 * First this method checks for movement inputs and calls the 
	 * respective methods if one has been found.
	 * 
	 * Otherwise this method calculates the necessary force to
	 * apply to the character to make him hover.
	 * 
	 * @param DeltaTime time since last tick
	 */
	void Hover(float DeltaTime);

	/**
	 * Moves the mesh of the character up and down
	 * to simulate hovering.
	 */
    void BobVertically();

    // ========== Movement Abilities ========== //

	/**
	 * Applies the movement input of the player to 
	 * the character.
	 * 
	 * @param Value input value 
	 */
	void Move(const FInputActionValue &Value);

	/**
	 * Stops the movement.
	 * Resets the velocity and acceleration.
	 */
	void Stop();

	/**
	 * Checks timers and cooldowns for all abilities
	 * 
	 * This method modifies the global variables for every ability
	 * according to their active cooldowns. If a cooldown is still
	 * active the character cannot perform the respective action.
	 * If a cooldown is over the ability is made active again.
	 */
	void CheckTimers();

	/**
	* Recalculates the logTime when Moving
	*/
	void UpdateSpeed();

	// ----- Jump ----- //

	/**
	 * Returns if the character is allowed to jump
	 * 
	 * @return true character may jump
	 * @return false character may not jump
	 */
	bool CanJump();

	/**
	 * Notifies the objects components that a jump
	 * input has been made.
	 */
	void StartJumping();

	/**
	 * Cancels all current jump inputs and resets
	 * the characters jumping
	 */
	void StopJumping();

	/**
	 * Perform a jump
	 * 
	 * This method handles grounded and aerial jumping and
	 * modifies cooldown and jump counter.
	 */
	void Jump();

	// ----- Dash ----- //

	/**
	 * Returns if the character is allowed to dash
	 * 
	 * @return true character may dash
	 * @return false character may not dash
	 */
	bool CanDash();

	/**
	 * Notifies the objects components that a dash
	 * input has been made.
	 */
	void StartDashing();

	/**
	 * Cancels all current dash inputs and resets
	 * the characters dashing
	 */
	void StopDashing();

	/**
	 * Perform a dash
	 * 
	 * This method handles the dash and its
	 * cooldown.
	 */
	void Dash();

	// ----- Crouch ----- //

	/**
	 * Makes the character crouch.
	 * The hitbox is halved.
	 */
	void StartCrouching();

	/**
	 * Makes the character stop crouching.
	 */
	void StopCrouching();

	// ----- Trick ----- //

	/**
	 * Returns if the character can perform a trick.
	 * 
	 * @return true can perform a trick
	 * @return false can not perform a trick
	 */
	bool CanDoTrick();

	/**
	 * Starts the time window for
	 * the player to perform a trick.
	 */
	void StartTrickWindow();

	/**
	 * Starts the timer while character is doing a trick.
	 */
	void StartTrickStateTimer();

	/**
	 * Strats the execution of a trick.
	 */
	void StartTrick();

	/**
	 * Ends the execution of a trick.
	 */
	void StopTrick();

	/**
	 * Performs a trick.
	 */
	void Trick();

public:

	/**
	 * @brief Called every tick
	 * 
	 * Calls all functions that need to be performed
	 * every tick. This includes hovering and checking
	 * cooldowns
	 * 
	 * @param DeltaTime time since last call 
	 */
	virtual void Tick(float DeltaTime) override;

};

