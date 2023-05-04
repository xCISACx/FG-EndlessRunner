// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PauseMenu.generated.h"

/**
 * 
 */
UCLASS()
class FG_ENDLESSRUNNER_API UPauseMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* ResumeButton;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* RestartButton;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UButton* MainMenuButton;
	
	virtual void NativeConstruct() override;

	UFUNCTION()
	void ResumeGame();

	UFUNCTION()
	void RestartGame();
	
	UFUNCTION()
	void MainMenu();
	
};
