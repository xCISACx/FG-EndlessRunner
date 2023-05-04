// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BaseObstacle.h"
#include "FG_EndlessRunnerCharacter.h"
#include "InputManager.h"
#include "WalkingPlane.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/GameModeBase.h"
#include "FG_EndlessRunnerGameMode.generated.h"

class AGroundTile;
class AGroundTilesParent;

UCLASS(minimalapi)
class AFG_EndlessRunnerGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AFG_EndlessRunnerGameMode();

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AInputManager> InputManagerClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AGroundTile> GroundTileClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AGroundTilesParent> GroundTilesParentClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AWalkingPlane> WalkingPlaneClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<ABaseObstacle> ShortObstacleClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<ABaseObstacle> TallObstacleClass;
	
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<UUserWidget> PauseMenuClass;
	
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<UUserWidget> GameOverClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<UUserWidget> UIBlueprint;

	/*UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<UUserWidget> UIWidgetClass;*/

	/*UPROPERTY(EditAnywhere, Category = "Config")
	UTextBlock* LivesText;*/

	UPROPERTY(EditAnywhere, Category = "Config")
	int32 NumInitialGroundTiles = 10;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	FTransform NextSpawnPoint;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	TArray<float> LaneSwitchValues;
	
	UPROPERTY(EditAnywhere, Category = "Runtime")
	int CurrentLaneIndexP1;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	int CurrentLaneIndexP2;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	float LanesSwitchingSpeed = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	float LanesSwitchingCooldown = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	float TallObstacleZOffset = 150.f;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	AStaticMeshActor* Plane; 

	UPROPERTY(EditAnywhere, Category = "Config", meta = (AllowPrivateAccess = true))
	AGroundTilesParent* GroundTilesParent;

	UPROPERTY(EditAnywhere, Category = "Config")
	AWalkingPlane* WalkingPlane;

	UPROPERTY(EditAnywhere, Category = "Config")
	FVector WalkingPlaneSpawnLocation;

	UPROPERTY(EditAnywhere, Category = "Config")
	AInputManager* InputManager;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	float ScoreP1 = 0;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	float ScoreP2 = 0;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	int ScoreIncrease;

	UPROPERTY(EditAnywhere, Category = "Config")
	float Speed = 600.f;

	UPROPERTY(EditAnywhere, Category = "Config")
	float MaxSpeed = 6000.f;

	UPROPERTY(EditAnywhere, Category = "Config")
	float SpeedIncrease = 0.1f;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	TArray<AGroundTile*> GroundTiles;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	AFG_EndlessRunnerCharacter* Player;

	UPROPERTY(EditAnywhere, Category = "Config")
	UUserWidget* UI;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	int LivesP1 = 3;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	int LivesP2 = 3;
	
	UPROPERTY(EditAnywhere, Category = "Runtime")
	float InvincibilityDuration = 2.f;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	bool IsGameOver;

	UFUNCTION(BlueprintCallable)
	void TogglePhysics(bool Value);

	UFUNCTION(BlueprintCallable)
	void CreateInitialGroundTiles();

	UFUNCTION(BlueprintCallable)
	AGroundTile* SpawnGroundTile();
	
	UFUNCTION(BlueprintCallable)
	void SpawnObstacles(AGroundTile* Tile, int laneIndex);

	UFUNCTION(BlueprintCallable)
	void RemoveRandomObstacle();

	UFUNCTION(BlueprintCallable)
	void RecycleTile(AGroundTile* Tile);

	UFUNCTION(BlueprintCallable)
	void GameOver();
	void RestartLevel();

	UFUNCTION(BlueprintCallable)
	void UpdateLives(int PlayerID, int Value);

	UFUNCTION(BlueprintCallable)
	void UpdateScore(int PlayerID, float Value);

	UFUNCTION(BlueprintCallable)
	void TogglePlayerInvincibility(int PlayerID, bool Value);

	UFUNCTION(BlueprintCallable)
	void ClearObstacles(AGroundTile* Tile);
	void PauseGame();

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};



