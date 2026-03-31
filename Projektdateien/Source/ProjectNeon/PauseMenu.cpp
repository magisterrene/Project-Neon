// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenu.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

void UPauseMenu::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    APlayerController* Player = UGameplayStatics::GetPlayerController(this, 0);
	Player->SetShowMouseCursor(true);
}

void UPauseMenu::Resume()
{
    //set input back to game
	APlayerController* Player = UGameplayStatics::GetPlayerController(this, 0);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(Player, false);
	Player->SetShowMouseCursor(false);

	//unpause game
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	//remove widget
	RemoveFromParent();
	MarkAsGarbage();
}

void UPauseMenu::Restart()
{
    //open level
	UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);

	//set input back to game
	APlayerController* Player = UGameplayStatics::GetPlayerController(this, 0);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(Player, false);
	Player->SetShowMouseCursor(false);

	//unpause game
	UGameplayStatics::SetGamePaused(GetWorld(), false);

	//remove widget
	RemoveFromParent();
    RemoveFromRoot();
}

void UPauseMenu::OpenOptionsMenu()
{
	//widget creation based on following manual:
	//https://nerivec.github.io/old-ue4-wiki/pages/umg-referencing-umg-widgets-in-code.html

	if (OptionsClass)
	{
		OptionsWidget = CreateWidget<UOptionsMenu>(this, OptionsClass);
		if (OptionsWidget)
		{
			OptionsWidget->AddToViewport(1000);
		}
	}
}

void UPauseMenu::ReturnToMenu()
{
    UGameplayStatics::OpenLevel(this, TEXT("MainMenu"));

    //remove widget
	RemoveFromParent();
    RemoveFromRoot();
}
