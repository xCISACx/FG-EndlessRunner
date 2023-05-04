// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MainMenu.generated.h"

/**
 * 
 */
class UTextBlock;
UCLASS()
class FG_ENDLESSRUNNER_API UMainMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* StartButton;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* QuitButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* HighscoreText;
	
	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void StartGame();

	UFUNCTION()
	void QuitGame();
	
};
