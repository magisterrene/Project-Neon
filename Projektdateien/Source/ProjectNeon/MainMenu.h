// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuButton.h"
#include "OptionsMenu.h"
#include "LevelSelectionMenu.h"
#include "MainMenu.generated.h"

/**
 * Class for the main menu.
 * The main menu can open the level selection,
 * the options menu and quit the game.
 */
UCLASS()
class PROJECTNEON_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:

	/**
	* Initializes the main menu.
	* All click events for the buttons are set.
	*/
	virtual void NativeOnInitialized() override;

	/** Class of the options menu widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> OptionsClass;

	/** 
	 * The widget containing the options menu.
	 * This value has to be set in the blueprint editor. 
	 */
	UPROPERTY()
	UOptionsMenu* OptionsWidget;

	/** Class of the level selection widget */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> LevelSelectionClass;

	/** 
	 * The widget containing the options menu.
	 * This value has to be set in the blueprint editor. 
	 */
	UPROPERTY()
	UUserWidget* LevelSelectionWidget;
	
private:

	/**
	* Handles the click on the options button.
	* Opens the options menu.
	*/
	UFUNCTION(BlueprintCallable)
	void OpenOptionsMenu();

	/**
	 * Handles the click on the level selection button.
	 * Opens the level selection menu.
	 */
	UFUNCTION(BlueprintCallable)
	void OpenLevelSelectionMenu();

	/**
	* Handles the click on the exit button.
	* Quits the game completely.
	*/
	UFUNCTION(BlueprintCallable)
	void ExitGame();
	
};