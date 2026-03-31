// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MenuButton.h"
#include "Blueprint/UserWidget.h"
#include "PopUp.generated.h"

DECLARE_DELEGATE(FOnDiscardSettings);

/**
 * Pop up class created when the user tries to exit
 * the options with uncommitted changes.
 * Can either discard the changes or return to options.
 */
UCLASS()
class PROJECTNEON_API UPopUp : public UUserWidget
{
	GENERATED_BODY()

protected:

	/**
	* Initializes the pop up.
	* All click events for the buttons are set.
	*/
	virtual void NativeOnInitialized() override;

	/**
	 * Button to discard changes and exit to main menu.
	 */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* DiscardChangesMenuButton;

	/**
	 * Button to return to the options menu.
	 */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UMenuButton* ReturnMenuButton;

private:

	/**
	* Handles a click on the discard changes button.
	*/
	UFUNCTION()
	void HandleDiscardChangesButtonClicked();

	/**
	* Handles the click on the return button.
	*/
	UFUNCTION()
	void HandleReturnButtonClicked();

public:

	/**
	 * Delegate broadcasting the result of users decision.
	 */
	FOnDiscardSettings OnDiscardSettings;
	
};