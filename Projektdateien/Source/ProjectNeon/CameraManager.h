// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectNeonCharacter.h"
#include "GameFramework/Actor.h"
#include <GameFramework/SpringArmComponent.h>
#include <Camera/CameraComponent.h>
#include "CameraManager.generated.h"

/**
 * @brief Class for the management of the player camera.
 * 
 * This class represents a camera on a camera boom.
 * The camera is pointing sideways to the character and
 * follows his movement.
 */
UCLASS()
class PROJECTNEON_API ACameraManager : public AActor
{
	GENERATED_BODY()

	/** Camera Boom positioning the camera to the side of the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
public:

	/**
	 * @brief Constructor for a CameraManager object.
	 * 
	 * Creates the components and sets their default values.
	 */
	ACameraManager();

protected:
	
	/**
	 * @brief Called on spawn.
	 */
	virtual void BeginPlay() override;

	/**
	 * @brief Calculate the position of the camera.
	 * 
	 * Calculates the position for the camera based on the characters position.
	 */
	UFUNCTION(BlueprintCallable)
	void UpdatePosition(AActor* target, bool bLockHeight, float interpSpeed, float cameraOffset);

public:	
	
	/**
	 * @brief Called every tick.
	 * 
	 * @param DeltaTime 
	 */
	virtual void Tick(float DeltaTime) override;

	/**
	 * @brief Get the Camera Boom subobject
	 * 
	 * @return camera boom object 
	 */
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	/**
	 * @brief Get the Follow Camera object
	 * 
	 * @return camera object 
	 */
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

};
