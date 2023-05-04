// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "GameOver.generated.h"

class UButton;
/**
 * 
 */
UCLASS()
class FG_ENDLESSRUNNER_API UGameOver : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* MainMenuButton;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* RestartButton;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UButton* QuitButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* PlayerWinText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	UTextBlock* ScoreText;

	virtual void NativeConstruct() override;
	
	UFUNCTION()
	void MainMenu();

	UFUNCTION()
	void RestartGame();

	UFUNCTION()
	void QuitGame();
	
};
