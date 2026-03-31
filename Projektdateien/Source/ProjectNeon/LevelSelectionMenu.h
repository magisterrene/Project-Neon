// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LevelSelectionMenu.generated.h"

/**
 * Class for the level selection menu.
 * This menu can open all levels and display
 * information about them.
 */
UCLASS()
class PROJECTNEON_API ULevelSelectionMenu : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY()
	float levelOne = -1.0f;

	UPROPERTY()
	float levelTwo = -1.0f;

	UPROPERTY()
	float levelThree = -1.0f;
	
protected:

	/**
	 * Initializer for the level selection menu.
	 * Uses the default function for initialization.
	 */
	virtual void NativeOnInitialized() override;

private:

	/**
	 * Opens the level with the given name.
	 *
	 * @param LevelName Name of the level to open
	 */
	UFUNCTION(BlueprintCallable)
	void OpenLevel(FName LevelName);

	/**
	 * Closes the level selection menu.
	 * Afterwards the underlying menu is shown again.
	 */
	UFUNCTION(BlueprintCallable)
	void CloseMenu();

	UFUNCTION(BlueprintCallable)
	float GetLevelBestTime(int LevelIdx);

};
