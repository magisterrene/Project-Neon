// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "TimeSave.generated.h"

/**
 *
 */
UCLASS()
class PROJECTNEON_API UTimeSave : public USaveGame
{
	GENERATED_BODY()

public:

	UTimeSave();

	UPROPERTY()
	float level1;

	UPROPERTY()
	float level2;

	UPROPERTY()
	float level3;
};
