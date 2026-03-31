// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsMenu.h"
#include "PauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTNEON_API UPauseMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
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

	UFUNCTION(BlueprintCallable)
	void Resume();

	UFUNCTION(BlueprintCallable)
	void Restart();

	/**
	* Handles the click on the options button.
	* Opens the options menu.
	*/
	UFUNCTION(BlueprintCallable)
	void OpenOptionsMenu();

	UFUNCTION(BlueprintCallable)
	void ReturnToMenu();
	
};
