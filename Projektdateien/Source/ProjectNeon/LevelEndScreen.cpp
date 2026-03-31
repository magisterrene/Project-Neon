// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelEndScreen.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

void ULevelEndScreen::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    APlayerController* Player = UGameplayStatics::GetPlayerController(this, 0);
	Player->SetShowMouseCursor(true);
}

void ULevelEndScreen::Restart()
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

void ULevelEndScreen::ReturnToMenu()
{
    UGameplayStatics::OpenLevel(this, TEXT("MainMenu"));

    //remove widget
	RemoveFromParent();
    RemoveFromRoot();
}