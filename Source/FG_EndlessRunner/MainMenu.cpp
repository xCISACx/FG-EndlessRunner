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
	
	UEndlessRunnerSaveGame* SaveGameInstance = Cast<UEndlessRunnerSaveGame>(UGameplayStatics::LoadGameFromSlot("MySave", 0));
	
	if (SaveGameInstance)
	{
		HighscoreText->SetText(FText::FromString("High Score: " + FString::FromInt(SaveGameInstance->HighScore)));
	}
	else
	{
		HighscoreText->SetText(FText::FromString("High Score: " + FString::FromInt(0)));
	}
}

void UMainMenu::StartGame()
{
	UWorld* World = GetWorld();

	if(World)
	{
		UGameplayStatics::OpenLevel(World, TEXT("ThirdPersonMap"));
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

