// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuGameModeBase.h"

#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

void AMainMenuGameModeBase::BeginPlay()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;

	if (MainMenuClass)
	{
		UUserWidget* Widget = CreateWidget(GetWorld(), MainMenuClass);

		if (Widget)
		{
			Widget->AddToViewport();
		}
	}
}

