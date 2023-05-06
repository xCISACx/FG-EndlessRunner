// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOver.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UGameOver::NativeConstruct()
{
	if(MainMenuButton)
	{
		MainMenuButton->OnClicked.AddDynamic(this, &UGameOver::MainMenu);
	}

	if(RestartButton)
	{
		RestartButton->OnClicked.AddDynamic(this, &UGameOver::RestartGame);
	}

	if(QuitButton)
	{
		QuitButton->OnClicked.AddDynamic(this, &UGameOver::QuitGame);
	}
}

void UGameOver::MainMenu()
{
	UWorld* World = GetWorld();

	if(World)
	{
		UGameplayStatics::OpenLevel(World, TEXT("MainMenu"));
	}
}

void UGameOver::RestartGame()
{
	UWorld* World = GetWorld();

	if(World)
	{
		UGameplayStatics::OpenLevel(World, TEXT("ThirdPersonMap"));
	}
}

void UGameOver::QuitGame()
{
	UWorld* World = GetWorld();

	if(World)
	{
		UKismetSystemLibrary::QuitGame(this, 0, EQuitPreference::Quit, false);
	}
}

