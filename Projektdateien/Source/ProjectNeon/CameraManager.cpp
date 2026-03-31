// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraManager.h"

ACameraManager::ACameraManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	//CameraBoom->Set(RootComponent);
	SetRootComponent(CameraBoom);
	CameraBoom->TargetArmLength = 750.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller
	CameraBoom->bDoCollisionTest = false; //disable collision testing

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void ACameraManager::BeginPlay()
{
	Super::BeginPlay();
}

void ACameraManager::UpdatePosition(AActor* target, bool bLockHeight, float interpSpeed, float cameraOffset)
{
	//get parameters
	FVector currentLocation = this->GetActorLocation();
	FVector targetLocation = target->GetActorLocation();
	targetLocation.Z += 100;
	FVector targetForwardVector = target->GetActorForwardVector();
	float delta = GetWorld()->GetDeltaSeconds();

	if (bLockHeight)
	{
		if (currentLocation.Z > 500 || currentLocation.Z < -500)
			targetLocation.Y -= 500;
	}

	targetForwardVector = targetForwardVector * cameraOffset;
	targetForwardVector.Y = 0;

	targetLocation = targetLocation + targetForwardVector;
	
	//interpolate value for smooth movement
	FVector destination = FMath::VInterpTo(currentLocation, targetLocation, delta, interpSpeed);

	this->SetActorLocation(destination);
}

void ACameraManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

