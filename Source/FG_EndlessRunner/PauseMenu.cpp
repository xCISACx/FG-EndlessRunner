// Fill out your copyright notice in the Description page of Project Settings.


#include "PauseMenu.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UPauseMenu::NativeConstruct()
{
	if (ResumeButton)
	{
		ResumeButton->OnClicked.AddDynamic(this, &UPauseMenu::ResumeGame);
	}

	if (RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UPauseMenu::RestartGame);
	}

	if (MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UPauseMenu::MainMenu);
	}
}

void UPauseMenu::ResumeGame()
{
	UGameplayStatics::SetGamePaused(GetWorld(), false);
	RemoveFromParent();
}

void UPauseMenu::RestartGame()
{
	// Get the current level's name
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);

	// Open the level again to reset it
	UGameplayStatics::OpenLevel(this, *CurrentLevelName);
}

void UPauseMenu::MainMenu()
{
	UWorld* World = GetWorld();

	if(World)
	{
		UGameplayStatics::OpenLevel(World, TEXT("MainMenu"));
	}
}

