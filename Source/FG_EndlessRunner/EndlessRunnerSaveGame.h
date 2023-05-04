// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "EndlessRunnerSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class FG_ENDLESSRUNNER_API UEndlessRunnerSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, Category = "SaveGame")
	int32 HighScore;

	UEndlessRunnerSaveGame();

	static UEndlessRunnerSaveGame* LoadScore();

	void SaveScore(int32 NewHighScore);
	
};
