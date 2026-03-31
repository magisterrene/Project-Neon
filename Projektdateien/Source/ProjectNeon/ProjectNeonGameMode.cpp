// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectNeonGameMode.h"
#include "ProjectNeonCharacter.h"
#include "TimeSave.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AProjectNeonGameMode::AProjectNeonGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(
		TEXT("/Game/ProjectNeon/Character/Blueprints/BP_Hoverman")
		);
	
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AProjectNeonGameMode::GetViewportFocus(bool &bIsFocused)
{
    if (GEngine) 
    {
        bIsFocused = GEngine->GameViewport->Viewport->IsForegroundWindow();
    }
}

float AProjectNeonGameMode::GetLastTime()
{
	return LastTime;
}

float AProjectNeonGameMode::GetCurrentTimer()
{
    return GetWorldTimerManager().GetTimerElapsed(TimerHandle);
}

void AProjectNeonGameMode::StartTimer()
{
    GetWorldTimerManager().SetTimer(TimerHandle, 9999.0f, false);
}

void AProjectNeonGameMode::StopTimer(int levelIndex)
{
    UTimeSave* SaveGameInstance = Cast<UTimeSave>(UGameplayStatics::CreateSaveGameObject(UTimeSave::StaticClass()));
	SaveGameInstance = Cast<UTimeSave>(UGameplayStatics::LoadGameFromSlot(TEXT("Highscore"), 0));
	if (!SaveGameInstance)
	{
		SaveGameInstance = Cast<UTimeSave>(UGameplayStatics::CreateSaveGameObject(UTimeSave::StaticClass()));
	}
	float thisTime = GetWorldTimerManager().GetTimerElapsed(TimerHandle);
	OnTimerStopped.Broadcast();


	switch (levelIndex)
	{
	case 0:
		LastTime = SaveGameInstance->level1;
		if (LastTime > thisTime)
		{
			SaveGameInstance->level1 = thisTime;
		}
		else if (LastTime < 0.1f)
		{
			SaveGameInstance->level1 = thisTime;
		}
		break;

	case 1:
		LastTime = SaveGameInstance->level2;
        if (LastTime > thisTime)
        {
            SaveGameInstance->level2 = thisTime;
        } 
        else if (LastTime < 0.1f)
        {
            SaveGameInstance->level2 = thisTime;
        }
		break;
	case 2:
		LastTime = SaveGameInstance->level3;
		if (LastTime > thisTime)
		{
			SaveGameInstance->level3 = thisTime;
		}
		else if (LastTime < 0.1f)
		{
			SaveGameInstance->level3 = thisTime;
		}
		break;
	default:
		break;
	}

	UGameplayStatics::SaveGameToSlot(SaveGameInstance, TEXT("Highscore"), 0);
}

float AProjectNeonGameMode::GetLevelOneStar(int levelIndex)
{
	switch (levelIndex)
	{
	case 0: 
		return this->LevelOneOneStar;
	case 1:
		return this->LevelTwoOneStar;
	case 2:
		return this->LevelThreeOneStar;
	default:
		return 0;
	}
}

float AProjectNeonGameMode::GetLevelTwoStar(int levelIndex)
{
	switch (levelIndex)
	{
	case 0:
		return this->LevelOneTwoStar;
	case 1:
		return this->LevelTwoTwoStar;
	case 2:
		return this->LevelThreeTwoStar;
	default:
		return 0;
	}
}

float AProjectNeonGameMode::GetLevelThreeStar(int levelIndex)
{
	switch (levelIndex)
	{
	case 0:
		return this->LevelOneThreeStar;
	case 1:
		return this->LevelTwoThreeStar;
	case 2:
		return this->LevelThreeThreeStar;
	default:
		return 0;
	}
}
