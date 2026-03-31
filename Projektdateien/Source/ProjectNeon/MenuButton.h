// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "MenuButton.generated.h"

/**
 * Class representing a button.
 * The widget only contains a button and can
 * be used as such.
 *
 * Based on the tutorial by "Ariel Quinteros"
 * https://www.youtube.com/watch?v=LphdJ-zgMUQ&t=632s&ab_channel=ArielQuinteros
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FClick);

UCLASS()
class PROJECTNEON_API UMenuButton : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;

	/**
	 * @brief OnClicked event for this button
	 */
	UPROPERTY(BlueprintCallable)
	FClick OnClicked;

	/**
	 * @brief button component for button like behaviour
	 */
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* Button;

public:

	/**
	 * @brief Getter for the button of this widget.
	 * 
	 * @return reference to the button
	 */
	UFUNCTION(BlueprintCallable)
	UButton* GetButton();
	
};
