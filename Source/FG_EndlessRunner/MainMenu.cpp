// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "EndlessRunnerSaveGame.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

class UEndlessRunnerSaveGame;

void UMainMenu::NativeConstruct()
{
	if (StartButton)
	{
		StartButton->OnClicked.AddDynamic(this, &UMainMenu::StartGame);
	}

	if (QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitGame);
	}
	
	UEndlessRunnerSaveGame* SaveGameInstance = Cast<UEndlessRunnerSaveGame>(UGameplayStatics::LoadGameFromSlot("HighScore", 0));
	
	if (SaveGameInstance)
	{
		HighscoreText->SetText(FText::AsNumber(SaveGameInstance->HighScore));
	}
	else
	{
		HighscoreText->SetText(FText::AsNumber(0));
	}

	UEndlessRunnerSaveGame* SaveGame = UEndlessRunnerSaveGame::LoadScore();
	FString HighScore = "High Score: " + SaveGame->HighScore;
	
	HighscoreText->SetText(FText::FromString(HighScore));
}

void UMainMenu::StartGame()
{
	UWorld* World = GetWorld();

	if(World)
	{
		UGameplayStatics::OpenLevel(World, TEXT("Main"));
	}
}

void UMainMenu::QuitGame()
{
	UWorld* World = GetWorld();

	if(World)
	{
		UKismetSystemLibrary::QuitGame(this, 0, EQuitPreference::Quit, false);
	}
}

