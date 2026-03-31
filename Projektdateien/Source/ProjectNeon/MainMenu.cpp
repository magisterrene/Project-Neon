// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "CoreMinimal.h"
#include "Misc/CString.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Blueprint/UserWidget.h"



void UMainMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

// ---------- Button Functions ---------- //

void UMainMenu::OpenOptionsMenu()
{
	//widget creation based on following manual:
	//https://nerivec.github.io/old-ue4-wiki/pages/umg-referencing-umg-widgets-in-code.html
	
	if (OptionsClass)
	{
		OptionsWidget = CreateWidget<UOptionsMenu>(this, OptionsClass);
		if (OptionsWidget)
		{
			OptionsWidget->AddToViewport(50);
		}
	}
}

void UMainMenu::OpenLevelSelectionMenu()
{
	//widget creation based on following manual:
	//https://nerivec.github.io/old-ue4-wiki/pages/umg-referencing-umg-widgets-in-code.html
	
	if (LevelSelectionClass)
	{
		LevelSelectionWidget = CreateWidget<UUserWidget>(this, LevelSelectionClass);
		if (LevelSelectionWidget)
		{
			LevelSelectionWidget->AddToViewport(50);
		}
	}	
}

void UMainMenu::ExitGame()
{
	UKismetSystemLibrary::QuitGame(this, 0, EQuitPreference::Quit, false);
}