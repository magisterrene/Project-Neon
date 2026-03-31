// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ProjectNeonGameMode.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimerStoppedDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTimerStartedDelegate);

UCLASS(minimalapi)
class AProjectNeonGameMode : public AGameModeBase
{
	GENERATED_BODY()

    FTimerHandle TimerHandle;

    /** Delegate for stop of the timer */
	UPROPERTY(BlueprintAssignable)
	FOnTimerStoppedDelegate OnTimerStopped;

	UPROPERTY(BlueprintAssignable)
	FOnTimerStartedDelegate OnTimerStarted;

    // ----- Level Timer Thresholds ----- //

    /*___________________________________________LevelOne______________________________________________*/

	/** Time needed to beat for one star in level one */
	UPROPERTY(EditAnywhere)
	float LevelOneOneStar = 65.0f;

	/** Time needed to beat for two stars in level one */
	UPROPERTY(EditAnywhere)
	float LevelOneTwoStar = 55.0f;

	/** Time needed to beat for three stars in level one */
	UPROPERTY(EditAnywhere)
	float LevelOneThreeStar = 47.0f;

    /*___________________________________________LevelTwo______________________________________________*/

    /** Time needed to beat for one star in level two */
    UPROPERTY(EditAnywhere)
    float LevelTwoOneStar = 150.0f;

    /** Time needed to beat for two stars in level two */
    UPROPERTY(EditAnywhere)
    float LevelTwoTwoStar = 125.0f;

    /** Time needed to beat for three stars in level two */
    UPROPERTY(EditAnywhere)
    float LevelTwoThreeStar = 110.0f;

    /*___________________________________________LevelThree______________________________________________*/

    /** Time needed to beat for one star in level three */
    UPROPERTY(EditAnywhere)
    float LevelThreeOneStar = 160.0f;

    /** Time needed to beat for two stars in level three */
    UPROPERTY(EditAnywhere)
    float LevelThreeTwoStar = 130.0f;

    /** Time needed to beat for three stars in level three */
    UPROPERTY(EditAnywhere)
    float LevelThreeThreeStar = 110.0f;

    /** Time needed to beat for three stars in level three */
    UPROPERTY(EditAnywhere)
    float LastTime = 0.0f;

public:
	AProjectNeonGameMode();

    UFUNCTION(BlueprintPure)
    static void GetViewportFocus(bool& bIsFocused);

    /**
     * @brief Get the last needed time for the current Level
     * return last time in float
     */
    UFUNCTION(BlueprintCallable)
    float GetLastTime();

    /**
     * @brief Get the current level time
     * 
     * @return float time in seconds
     */
    UFUNCTION(BlueprintCallable)
    float GetCurrentTimer();

    void StartTimer();

    /**
     * @brief Stops the level timer and saves the level time
     * if a new best time was achieved
     */
    void StopTimer(int levelIndex);

    /**
     * @brief Get the time needed to beat for one star in level one
     * 
     * @return float time in seconds
     */
    UFUNCTION(BlueprintCallable)
    float GetLevelOneStar(int levelIndex);

    /**
     * @brief Get the time needed to beat for two stars in level one
     * 
     * @return float time in seconds
     */
    UFUNCTION(BlueprintCallable)
    float GetLevelTwoStar(int levelIndex);

    /**
     * @brief Get the time needed to beat for three stars in level one
     * 
     * @return float time in seconds
     */
    UFUNCTION(BlueprintCallable)
    float GetLevelThreeStar(int levelIndex);
};



