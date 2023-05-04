// Fill out your copyright notice in the Description page of Project Settings.


#include "EndlessRunnerSaveGame.h"

#include "Kismet/GameplayStatics.h"

const FString SaveSlotName = TEXT("MySave");
const uint32 UserIndex = 0;

UEndlessRunnerSaveGame::UEndlessRunnerSaveGame()
{
	HighScore = 0;
}

UEndlessRunnerSaveGame* UEndlessRunnerSaveGame::LoadScore()
{
	UEndlessRunnerSaveGame* SaveGame = Cast<UEndlessRunnerSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotName, UserIndex));

	if (!SaveGame)
	{
		SaveGame = Cast<UEndlessRunnerSaveGame>(UGameplayStatics::CreateSaveGameObject(StaticClass()));
	}
	return SaveGame;
}

void UEndlessRunnerSaveGame::SaveScore(int32 NewHighScore)
{
	if (NewHighScore > HighScore)
	{
		HighScore = NewHighScore;
		UGameplayStatics::SaveGameToSlot(this, SaveSlotName, UserIndex);
	}
}
