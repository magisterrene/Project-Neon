// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelSelectionMenu.h"
#include "TimeSave.h"
#include "Kismet/GameplayStatics.h"

void ULevelSelectionMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void ULevelSelectionMenu::OpenLevel(FName LevelName)
{
	UGameplayStatics::OpenLevel(this, LevelName);
}

void ULevelSelectionMenu::CloseMenu()
{
	RemoveFromParent();
	RemoveFromRoot();
}

float ULevelSelectionMenu::GetLevelBestTime(int LevelIdx)
{
	UTimeSave* SaveGameInstance = Cast<UTimeSave>(UGameplayStatics::LoadGameFromSlot(TEXT("Highscore"), 0));
	if (!SaveGameInstance)
	{
		SaveGameInstance = Cast<UTimeSave>(UGameplayStatics::CreateSaveGameObject(UTimeSave::StaticClass()));
	}
	this->levelOne = SaveGameInstance->level1;
	this->levelTwo = SaveGameInstance->level2;
	this->levelThree = SaveGameInstance->level3;
	
	float res = 0.0f;
	switch (LevelIdx)
	{
	case 0: 
		res = this->levelOne;
		break;
	case 1: 
		res = this->levelTwo;
		break;
	case 2:
		res = this->levelThree;
		break;
	default:
		break;
	}
	return res;
}