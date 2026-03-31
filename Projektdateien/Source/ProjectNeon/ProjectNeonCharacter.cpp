// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectNeonCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "CameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/PrimitiveComponent.h"
#include "HoverManMovementComponent.h"
#include "BouncyWall.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

// ========== Setup ========== //

// ----- Character ----- //

AProjectNeonCharacter::AProjectNeonCharacter(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UHoverManMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(CapsuleRadius, CapsuleHalfHeight);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Init list of last rotations
	InitLastRotation(0.0f);

    this->GameMode = Cast<AProjectNeonGameMode>(UGameplayStatics::GetGameMode(this));

	SetupCharacterMovement();
	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

float AProjectNeonCharacter::GetDashCooldown()
{
	return GetWorldTimerManager().GetTimerRemaining(DashTimerHandle);
}

float AProjectNeonCharacter::GetDashTimer()
{
	return DashTimer;
}

float AProjectNeonCharacter::GetSpeed()
{
	return fabs(GetVelocity().X);
}

bool AProjectNeonCharacter::GetTrickWindow()
{
	return GetWorldTimerManager().IsTimerActive(TrickTimerHandle);
}

bool AProjectNeonCharacter::GetMakingTrick()
{
	return GetWorldTimerManager().IsTimerActive(TrickStateTimerHandle);
}

void AProjectNeonCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	// Add timer handle for infinite speed increase
	GetWorldTimerManager().SetTimer(SpeedTimerHandle, this, &AProjectNeonCharacter::UpdateSpeed, 0.1f, true);

	// Set reference location of mesh for bobbing
	ReferenceLocation = GetMesh()->GetRelativeLocation();
}

void AProjectNeonCharacter::StartHover(void)
{
	bIsHovering = true;
	OnStartHovering.Broadcast();
}

void AProjectNeonCharacter::StopHover(void)
{
	// only stop if currently hovering
	if (bIsHovering) {
		bIsHovering = false;

		// make character fall smoothly by resetting velocity
		if (GetCharacterMovement()->Velocity.Z < 0.0f) {
			GetCharacterMovement()->Velocity.Z = 0.0f;
		}

		OnStopHovering.Broadcast();
	}
}

AProjectNeonCharacter::ListTraceResult AProjectNeonCharacter::TraceGround(void)
{
	ListTraceResult Result;
	Result.bHits.SetNum(NUM_TRACES);
	Result.HitPoints.SetNum(NUM_TRACES);

	// Get the current location of the character
    FVector CharacterLocation = GetActorLocation();

    // Adjust the character location to align with the ground surface
	CharacterLocation.Z -= CapsuleHalfHeight;

    // Create collision query parameters for the line traces
    FCollisionQueryParams TraceParams(FName(TEXT("GroundTrace")), false, this);
	
	
	// Calculate the points for line traces
	FVector ForwardVec = GetActorForwardVector();
	FVector Front = CharacterLocation + ForwardVec * (HoverBoardFront - 5);
	FVector Back = CharacterLocation - ForwardVec * (HoverBoardBack - 5);
	float Offset = (Front - Back).Length() / (NUM_TRACES - 1);

	// perform linetraces along the hoverboard
	for (int i = 0; i < NUM_TRACES; i++)
	{
		FVector Start = Front - ForwardVec * (Offset * i);
		FVector End = Start - FVector(0.0f, 0.0f, HoverHeight); 
		
		// Perform line traces to check for collisions
		FHitResult HitResult;
		Result.bHits[i] = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);
		Result.HitPoints[i] = HitResult.ImpactPoint;
		
		//DEBUG
		// DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.1f, 0, 2.0f);
	}

	return Result;
}

AProjectNeonCharacter::ListTraceResult AProjectNeonCharacter::TraceSky(void)
{
	ListTraceResult Result;
	Result.bHits.SetNum(NUM_TRACES);
	Result.HitPoints.SetNum(NUM_TRACES);

	// Get the current location of the character
	FVector CharacterLocation = GetActorLocation();

	// Adjust the character location to align with the ground surface
	CharacterLocation.Z -= CapsuleHalfHeight;

	// Create collision query parameters for the line traces
	FCollisionQueryParams TraceParams(FName(TEXT("SkyTrace")), false, this);
	
	// Calculate the points for line traces
	FVector ForwardVec = GetActorForwardVector();
	FVector Front = CharacterLocation + ForwardVec * (HoverBoardFront - 5);
	FVector Back = CharacterLocation - ForwardVec * (HoverBoardBack - 5);
	float Offset = (Front - Back).Length() / (NUM_TRACES - 1);

	// perform linetraces along the hoverboard
	for (int i = 0; i < NUM_TRACES; i++)
	{
		FVector Start = Front - ForwardVec * (Offset * i);
		FVector End = Start + FVector(0.0f, 0.0f, HoverBoardTop);

		// Perform line traces to check for collisions
		FHitResult HitResult;
		Result.bHits[i] = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, TraceParams);
		Result.HitPoints[i] = HitResult.ImpactPoint;

		//DEBUG
		//DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.1f, 0, 2.0f);
	}

	return Result;
}

void AProjectNeonCharacter::CheckCollision(void)
{
	DoubleTraceResult Trace = TraceHitbox();
	FVector CharacterLocation = GetActorLocation();
	
	// collision if any of the traces hit something
	if (Trace.bFrontHit || Trace.bBackHit) {
		
		FVector ViewingDirection = FVector(GetActorForwardVector().X, 0.0f, 0.0f);
		ViewingDirection.Normalize();
		float Distance = 0.0f;

		if (Trace.bFrontHit)
		{
			// bounce of if hit a bouncy wall
			if (Trace.FrontHitActor->IsA(ABouncyWall::StaticClass()) )
			{
				GetCharacterMovement()->Velocity.X = GetVelocity().X * -1.2f;
				OnBouncyWallCollision.Broadcast();
			} else {
				GetCharacterMovement()->Velocity.X = 0.0f;
				OnCollision.Broadcast();
				Stop();
			}
			Distance = -ViewingDirection.X * HoverBoardFront - (CharacterLocation.X - Trace.FrontHitPoint.X);
		}
		else if (Trace.bBackHit)
		{
			// bounce of if hit a bouncy wall
			if (Trace.BackHitActor->IsA(ABouncyWall::StaticClass()))
			{
				GetCharacterMovement()->Velocity.X = GetVelocity().X * -1.2f;
				OnBouncyWallCollision.Broadcast();
			} else {
				GetCharacterMovement()->Velocity.X = 0.0f;
				OnCollision.Broadcast();
				Stop();
			}
			Distance = ViewingDirection.X * HoverBoardBack - (CharacterLocation.X - Trace.BackHitPoint.X);
		}

		SetActorLocation(FVector(CharacterLocation.X + Distance, CharacterLocation.Y, CharacterLocation.Z));
	}

	// check if the character hits a bounce wall
	DoubleTraceResult CharTrace = TraceCharHitbox();

	if ((CharTrace.bFrontHit && CharTrace.FrontHitActor->IsA(ABouncyWall::StaticClass()))
		|| (CharTrace.bBackHit && CharTrace.BackHitActor->IsA(ABouncyWall::StaticClass())))
	{
		GetCharacterMovement()->Velocity.X = GetVelocity().X * -1.2f;
		OnBouncyWallCollision.Broadcast();
	}

	// check for hit upwards
	ListTraceResult SkyTrace = TraceSky();
	if (CheckHit(SkyTrace)) {
		GetCharacterMovement()->Velocity.Z = UKismetMathLibrary::FMin(0.0f, GetCharacterMovement()->Velocity.Z);
		Stop();
		OnCollision.Broadcast();
	}
}

AProjectNeonCharacter::DoubleTraceResult AProjectNeonCharacter::TraceHitbox(void) 
{
	// Get the current location of the character
	FVector CharacterLocation = GetActorLocation();
	// Adjust the character location to align with the ground surface
	CharacterLocation.Z -= (CapsuleHalfHeight - 5);
	// Create collision query parameters for the line traces
	FCollisionQueryParams TraceParams(FName(TEXT("HitboxTrace")), false, this);

	// Calculate the forward and backward offsets from the character's location
	FRotator CharRot = GetActorRotation();
	float NewRotation = SumOfLastRotation();
	NewRotation /= NUM_LAST_ROTATIONS;
	CharRot.Pitch = NewRotation/2;
	FVector XAxis = FVector(1.0f, 0.0f, 0.0f);
	FVector ToFront = CharRot.RotateVector(XAxis * HoverBoardFront);
	FVector ToBack = CharRot.RotateVector(-XAxis * HoverBoardBack);
	FVector ForwardEnd = CharacterLocation + ToFront;
	FVector BackwardEnd = CharacterLocation + ToBack;
	
	//DEBUG
	//DrawDebugLine(GetWorld(), CharacterLocation, ForwardEnd, FColor::Green, false, 0.1f, 0, 2.0f);
    //DrawDebugLine(GetWorld(), CharacterLocation, BackwardEnd, FColor::Green, false, 0.1f, 0, 2.0f);

	FHitResult ForwardHitResult;
	FHitResult BackwardHitResult;

	bool bForwardHit = GetWorld()->LineTraceSingleByChannel(ForwardHitResult, CharacterLocation, ForwardEnd, ECC_Visibility, TraceParams);
	bool bBackwardHit = GetWorld()->LineTraceSingleByChannel(BackwardHitResult, CharacterLocation, BackwardEnd, ECC_Visibility, TraceParams);

	AActor* frontActor = ForwardHitResult.GetActor();
	AActor* backActor = BackwardHitResult.GetActor();

	return DoubleTraceResult(
		bForwardHit, bBackwardHit, 
		ForwardHitResult.ImpactPoint, BackwardHitResult.ImpactPoint,
		frontActor, backActor
	);
}

AProjectNeonCharacter::DoubleTraceResult AProjectNeonCharacter::TraceCharHitbox(void)
{
	// Get the current location of the character
	FVector CharacterLocation = GetActorLocation();
	// Adjust the character location to align with the ground surface
	if (!GetCharacterMovement()->IsCrouching())
	{
		CharacterLocation.Z += CapsuleHalfHeight;
	}
	
	// Create collision query parameters for the line traces
	FCollisionQueryParams TraceParams(FName(TEXT("HitboxCharTrace")), false, this);
	FVector XAxis = FVector(1.0f, 0.0f, 0.0f);

	FVector ToFront = XAxis * (CapsuleRadius + 10);
	FVector ToBack = -XAxis * (CapsuleRadius + 10);
	FVector ForwardEnd = CharacterLocation + ToFront;
	FVector BackwardEnd = CharacterLocation + ToBack;

	//DEBUG
	//DrawDebugLine(GetWorld(), CharacterLocation, ForwardEnd, FColor::Green, false, 0.1f, 0, 2.0f);
	//DrawDebugLine(GetWorld(), CharacterLocation, BackwardEnd, FColor::Green, false, 0.1f, 0, 2.0f);

	FHitResult ForwardHitResult;
	FHitResult BackwardHitResult;

	bool bForwardHit = GetWorld()->LineTraceSingleByChannel(ForwardHitResult, CharacterLocation, ForwardEnd, ECC_Visibility, TraceParams);
	bool bBackwardHit = GetWorld()->LineTraceSingleByChannel(BackwardHitResult, CharacterLocation, BackwardEnd, ECC_Visibility, TraceParams);

	AActor* frontActor = ForwardHitResult.GetActor();
	AActor* backActor = BackwardHitResult.GetActor();

	return DoubleTraceResult(
		bForwardHit, bBackwardHit,
		ForwardHitResult.ImpactPoint, BackwardHitResult.ImpactPoint,
		frontActor, backActor
	);
}

AProjectNeonCharacter::DoubleTraceResult AProjectNeonCharacter::TraceForRotation(void)
{
	// Get the current location of the character
	FVector CharacterLocation = GetActorLocation();
	// Adjust the character location to align with the ground surface
	CharacterLocation.Z -= CapsuleHalfHeight;
	// Create collision query parameters for the line traces
	FCollisionQueryParams TraceParams(FName(TEXT("RotationTrace")), false, this);

	FVector ForwardVec = GetActorForwardVector() * 75;
	// Calculate the forward and backward offsets from the character's location
	FVector ForwardStart = CharacterLocation + ForwardVec;
	FVector BackwardStart = CharacterLocation - ForwardVec;

	// Calculate the forward and backward offsets from the character's location
	FVector ForwardEnd = ForwardStart - FVector(0.0f, 0.0f, 500.0f);
	FVector BackwardEnd = BackwardStart - FVector(0.0f, 0.0f, 500.0f);

	//DEBUG
	//DrawDebugLine(GetWorld(), ForwardStart, ForwardEnd, FColor::Green, false, 0.1f, 0, 2.0f);
	//DrawDebugLine(GetWorld(), BackwardStart, BackwardEnd, FColor::Green, false, 0.1f, 0, 2.0f);

	FHitResult ForwardHitResult;
	FHitResult BackwardHitResult;

	bool bForwardHit = GetWorld()->LineTraceSingleByChannel(ForwardHitResult, ForwardStart, ForwardEnd, ECC_Visibility, TraceParams);
	bool bBackwardHit = GetWorld()->LineTraceSingleByChannel(BackwardHitResult, BackwardStart, BackwardEnd, ECC_Visibility, TraceParams);

	AActor* frontActor = ForwardHitResult.GetActor();
	AActor* backActor = BackwardHitResult.GetActor();

	return DoubleTraceResult(
		bForwardHit, bBackwardHit,
		ForwardHitResult.ImpactPoint, BackwardHitResult.ImpactPoint,
		frontActor, backActor
	);
}



// ----- Input ----- //

void AProjectNeonCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProjectNeonCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AProjectNeonCharacter::Stop);
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AProjectNeonCharacter::StartJumping);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AProjectNeonCharacter::StopJumping);

		// Dashing
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &AProjectNeonCharacter::StartDashing);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &AProjectNeonCharacter::StopDashing);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AProjectNeonCharacter::StartCrouching);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AProjectNeonCharacter::StopCrouching);

		//Trick
		EnhancedInputComponent->BindAction(TrickAction, ETriggerEvent::Started, this, &AProjectNeonCharacter::StartTrick);
		EnhancedInputComponent->BindAction(TrickAction, ETriggerEvent::Completed, this, &AProjectNeonCharacter::StopTrick);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component!"));
	}
}

void AProjectNeonCharacter::SetupCharacterMovement()
{
	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 750.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->MaxWalkSpeed = 1700.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;
	GetCharacterMovement()->GroundFriction = 0.0f;
	GetCharacterMovement()->AirControl = 1.0f;
	GetCharacterMovement()->MaxAcceleration = 200.0f;
}

// ========== Getters & Setters ========== //

float AProjectNeonCharacter::GetCurrentSpeed()
{
	// Get the character's movement component
    UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
    
    // Ensure the movement component is valid
    if (MovementComponent)
    {
        // Get the current velocity vector
        FVector CharacterVelocity = MovementComponent->Velocity;
        
        // Calculate the speed by taking the length of the velocity vector
        float Speed = CharacterVelocity.Size();
        
        return Speed;
    }
    
    // Return 0.0f if the movement component is not valid
    return 0.0f;
}

// ========== Movement ========== //

void AProjectNeonCharacter::Move(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		GetCharacterMovement()->Velocity.Y = 0.0f;

		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		float CurrentSpeed = GetCurrentSpeed();
		if (!bIsMoving) {
			bIsMoving = true;
			StartTime = GetWorld()->GetTimeSeconds();

			if (GetCharacterMovement()->MaxWalkSpeed < 5000) { 
				GetCharacterMovement()->MaxWalkSpeed = 1700 + GetCharacterMovement()->MaxWalkSpeed * LogTime; 
			}

			CurrentSpeed = FMath::FInterpTo(CurrentSpeed, GetCharacterMovement()->MaxWalkSpeed, GetWorld()->GetDeltaSeconds(), 5.0f);
			
		}

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.X * CurrentSpeed);
	}
}

void AProjectNeonCharacter::Stop()
{
	if (bIsMoving) {
		bIsMoving = false;
		GetCharacterMovement()->MaxWalkSpeed = 1700.0f;
		StartTime = 0;
	}
}

void AProjectNeonCharacter::UpdateSpeed()
{
	if (bIsMoving) {
		float elapsedTime = GetWorld()->GetTimeSeconds() - StartTime;
		LogTime = FMath::Loge(1 + elapsedTime) / 8;
	}
}

bool AProjectNeonCharacter::CheckHit(ListTraceResult Trace)
{
	bool bHovering = false;
	for (int i = 0; i < NUM_TRACES && !bHovering; i++)
	{
		if (Trace.bHits[i]) {
			bHovering = true;
		}
	}
	return bHovering;
}

float AProjectNeonCharacter::GetMaxGround(ListTraceResult Trace)
{
	float Ground = -9999.0f;
	for (int i = 0; i < NUM_TRACES; i++)
	{
		if (Trace.bHits[i] && Trace.HitPoints[i].Z > Ground) {
			Ground = Trace.HitPoints[i].Z;
		}
	}
	return Ground;
}

/**
 * Calculates the angle between two vectors in degrees.
 * 
 * @param Vector1 first vector
 * @param Vector2 second vector
 * @return float angle in degrees
 */
float CalculateAngleBetweenVectors(const FVector& Vector1, const FVector& Vector2)
{
	// Normalisiere die Vektoren, um korrekte Winkel zu erhalten
	FVector NormalizedVector1 = Vector1.GetSafeNormal();
	FVector NormalizedVector2 = Vector2.GetSafeNormal();

	// Berechne das Skalarprodukt der beiden normalisierten Vektoren
	float DotProduct = FVector::DotProduct(NormalizedVector1, NormalizedVector2);

	// Berechne den Winkel in Radian (acos gibt den Winkel in Radiant zur�ck)
	float AngleInRadians = FMath::Acos(DotProduct);

	// Winkel in Grad umrechnen
	float AngleInDegrees = FMath::RadiansToDegrees(AngleInRadians);

	return AngleInDegrees;
}

void AProjectNeonCharacter::UpdateLastRotation(float NewRotation)
{
	for (int i = 0; i < NUM_LAST_ROTATIONS - 1; i++)
	{
		LastRotation[i + 1] = LastRotation[i];
	}
	LastRotation[0] = NewRotation;
}

float AProjectNeonCharacter::SumOfLastRotation(void) 
{
	float Sum = 0.0f;
	for (int i = 0; i < NUM_LAST_ROTATIONS; i++)
	{
		Sum += LastRotation[i];
	}
	return Sum;
}

void AProjectNeonCharacter::InitLastRotation(float initVal)
{
	LastRotation.SetNum(NUM_LAST_ROTATIONS);
	for (int i = 0; i < NUM_LAST_ROTATIONS; i++)
	{
		LastRotation[i] += initVal;
	}
}

void AProjectNeonCharacter::Hover(float DeltaTime)
{
    ListTraceResult Trace = TraceGround();
	
	// Calculate the target hover height based on the collisions
    if (CheckHit(Trace))
    {
		AerialJumpCounter = 1;

        float Ground = GetMaxGround(Trace);
        // Move the character to the target hover height if hovering
        if (bIsHovering)
        {
			//DrawDebugPoint(GetWorld(), FVector(GetActorLocation().X, GetActorLocation().Y, Ground + HoverHeight + CapsuleHalfHeight), 5.0f, FColor::Red, false, 1.0f);
			FVector CharacterLocation = GetActorLocation();
			float TargetHeight = Ground + HoverHeight + CapsuleHalfHeight;
			if (CharacterLocation.Z < TargetHeight)
			{
				SetActorLocation(FVector(
					CharacterLocation.X, 
					CharacterLocation.Y, 
					UKismetMathLibrary::FMin(TargetHeight, CharacterLocation.Z + 20)));
				GetCharacterMovement()->Velocity.Z = 
					UKismetMathLibrary::FMax(0.0f, GetCharacterMovement()->Velocity.Z);
			} else {
				SetActorLocation(FVector(CharacterLocation.X, CharacterLocation.Y, TargetHeight));
			}
			
        }

		AProjectNeonCharacter::DoubleTraceResult RotTrace = TraceForRotation();
		FRotator TargetRotation = GetActorRotation();
		FVector ForwardVec = GetActorForwardVector();
		float NewRotation;
		if (RotTrace.bFrontHit && RotTrace.bBackHit)
		{
			FVector PitchVec = RotTrace.FrontHitPoint - RotTrace.BackHitPoint;
			PitchVec.Normalize();
			FVector XYProjection = FVector(PitchVec.X, PitchVec.Y, 0.0f);
			XYProjection.Normalize();
			NewRotation = CalculateAngleBetweenVectors(XYProjection, PitchVec);

			if (RotTrace.FrontHitPoint.Z > RotTrace.BackHitPoint.Z)
			{
				NewRotation = TargetRotation.Pitch + NewRotation;
			} else {
				NewRotation = TargetRotation.Pitch - NewRotation;
			}
			if (NewRotation > 46.0f || NewRotation < -46.0f)
				{
					NewRotation = 0.0f;
				}
			
		} else {
			NewRotation = 0.0f;
		}
		NewRotation += SumOfLastRotation();
		NewRotation /= NUM_LAST_ROTATIONS + 1;
		TargetRotation.Pitch = NewRotation;
		UpdateLastRotation(TargetRotation.Pitch);
		SetActorRotation(TargetRotation);

        // Start hovering if the character is falling downwards
        if (GetCharacterMovement()->Velocity.Z < 0.0f)
        {
			if (!bIsHovering && !GetCharacterMovement()->IsCrouching())
			{
				StartTrickWindow();
			}
            StartHover();
        }
    }
    else
    {
		if (bIsHovering) {
			if (GetWorldTimerManager().IsTimerActive(JumpWindowTimerHandle)) {
				GetWorldTimerManager().ClearTimer(JumpWindowTimerHandle);
			}
			GetWorldTimerManager().SetTimer(JumpWindowTimerHandle, 0.1f, false);
		}

        // Stop hovering if there are no collisions
        StopHover();
		UpdateLastRotation(0.0f);
    }
}

void AProjectNeonCharacter::BobVertically()
{
	float time = GetGameTimeSinceCreation();
	float bobbingFactor = BobbingDistance * sinf(time * BobbingSpeed);
	FVector newLocation = ReferenceLocation;
	newLocation.Z += bobbingFactor;
	GetMesh()->SetRelativeLocation(newLocation);
}

// ========== Abilities ========== //

void AProjectNeonCharacter::CheckTimers()
{
	bCanJump = !GetWorldTimerManager().IsTimerActive(JumpTimerHandle);
	bCanDash = !GetWorldTimerManager().IsTimerActive(DashTimerHandle);
}

// ----- Jump ----- //

void AProjectNeonCharacter::StartJumping()
{
    bShouldJump = true;
}

void AProjectNeonCharacter::StopJumping()
{
    bShouldJump = false;
    Super::StopJumping();
}

bool AProjectNeonCharacter::CanJump()
{
	ListTraceResult Trace = TraceGround();
	return (!GetWorldTimerManager().IsTimerActive(JumpTimerHandle) && (CheckHit(Trace) 
		|| GetWorldTimerManager().IsTimerActive(JumpWindowTimerHandle))) || AerialJumpCounter > 0;
}

void AProjectNeonCharacter::Jump()
{
	LaunchCharacter(GetActorUpVector() * GetCharacterMovement()->JumpZVelocity, false, true);
	ListTraceResult Trace = TraceGround();

	if  (CheckHit(Trace))
	{
		GetWorldTimerManager().SetTimer(JumpTimerHandle, 0.5, false);
	}
	else
	{
		AerialJumpCounter--;
	}
	OnJumpPerformed.Broadcast();
}

// ----- Dash ----- //

bool AProjectNeonCharacter::CanDash()
{
	return bCanDash && !bIsHovering;
}

void AProjectNeonCharacter::StartDashing()
{
    if (CanDash()) {
		bShouldDash = true;
	}
}

void AProjectNeonCharacter::StopDashing()
{
    bShouldJump = false;
}

void AProjectNeonCharacter::Dash()
{
	LaunchCharacter(GetActorForwardVector() * DashStrength, true, false);
	GetWorldTimerManager().SetTimer(DashTimerHandle, DashTimer, false);
	OnDashPerformed.Broadcast();
}

// ----- Dash ----- //

void AProjectNeonCharacter::StartCrouching()
{
	Crouch();
}

void AProjectNeonCharacter::StopCrouching()
{
	UnCrouch();
}

bool AProjectNeonCharacter::CanDoTrick()
{
    return GetWorldTimerManager().IsTimerActive(TrickTimerHandle) && bIsHovering && !GetCharacterMovement()->IsCrouching();
}

void AProjectNeonCharacter::StartTrickWindow()
{
	GetWorldTimerManager().SetTimer(TrickTimerHandle, 0.3, false);
}

void AProjectNeonCharacter::StartTrickStateTimer()
{
	GetWorldTimerManager().SetTimer(TrickStateTimerHandle, 0.6f, false);
}

void AProjectNeonCharacter::StartTrick()
{
	if (CanDoTrick())
	{
		bShouldTrick = true;
	}
}

void AProjectNeonCharacter::StopTrick()
{
	bShouldTrick = false;
}

void AProjectNeonCharacter::Trick()
{
	GetWorldTimerManager().ClearTimer(TrickTimerHandle);
	if (GetCharacterMovement()->Velocity.X < 0.0f) {
		GetCharacterMovement()->Velocity.X -= TrickStrength;
	} else {
		GetCharacterMovement()->Velocity.X += TrickStrength;
	}
	StartTrickStateTimer();
	OnTrickPerformed.Broadcast();
}

// ==================== Timer ================//
void AProjectNeonCharacter::StartTimer()
{
	if (!bIsTimerStarted) {
		bIsTimerStarted = true;
        this->GameMode->StartTimer();
	}
}

void AProjectNeonCharacter::StopTimer(int Levelidx)
{
	if (bIsTimerStarted)
	{
		this->GameMode->StopTimer(Levelidx);
	}
	bIsTimerStarted = false;
}

// ========== Tick ========== //

void AProjectNeonCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckTimers();
	CheckCollision();
	if (bIsMoving) {
		if (GetCharacterMovement()->MaxWalkSpeed < 5000)
		{
			GetCharacterMovement()->MaxWalkSpeed = 1700 + GetCharacterMovement()->MaxWalkSpeed * LogTime;
		}
	}
	// Check for jump input
	if (CanJump() && bShouldJump)
	{
		bShouldJump = false;
		if (bIsHovering) StopHover();
		Jump();
	}
	// Check for dash input
	else if (CanDash() && bShouldDash)
	{
		bShouldDash = false;
		Dash();
	}
	// Check for trick input
	else if (bShouldTrick)
	{
		bShouldTrick = false;
		Trick();
	}
	else
	{
		Hover(DeltaTime);
	}

	BobVertically();
}