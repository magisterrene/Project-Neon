// Fill out your copyright notice in the Description page of Project Settings.


#include "PopUp.h"
#include "MenuButton.h"

void UPopUp::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	//add button methods
	DiscardChangesMenuButton->GetButton()->OnClicked.AddDynamic(this, &UPopUp::HandleDiscardChangesButtonClicked);
	ReturnMenuButton->GetButton()->OnClicked.AddDynamic(this, &UPopUp::HandleReturnButtonClicked);
}

void UPopUp::HandleDiscardChangesButtonClicked()
{
	//notify that settings should be discarded
	OnDiscardSettings.ExecuteIfBound();

	//remove widget
	RemoveFromParent();
    RemoveFromRoot();
}

void UPopUp::HandleReturnButtonClicked()
{
	//remove widget
	RemoveFromParent();
    RemoveFromRoot();
}