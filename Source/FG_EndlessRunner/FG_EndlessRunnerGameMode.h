// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FG_EndlessRunnerCharacter.h"
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
	TSubclassOf<AGroundTile> GroundTileClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AGroundTilesParent> GroundTilesParentClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AWalkingPlane> WalkingPlaneClass;

	UPROPERTY(EditAnywhere, Category = "Config")
	int32 NumInitialGroundTiles = 10;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	FTransform NextSpawnPoint;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	TArray<float> LaneSwitchValues;
	
	UPROPERTY(EditAnywhere, Category = "Runtime")
	int CurrentLaneIndex;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	bool CanSwitchLanes;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	float LanesSwitchingSpeed = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	float LanesSwitchingCooldown = 0.5f;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	AStaticMeshActor* Plane; 

	UPROPERTY(EditAnywhere, Category = "Config", meta = (AllowPrivateAccess = true))
	AGroundTilesParent* GroundTilesParent;

	UPROPERTY(EditAnywhere, Category = "Config")
	AWalkingPlane* WalkingPlane;

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

	UFUNCTION(BlueprintCallable)
	void CreateInitialGroundTiles();

	UFUNCTION(BlueprintCallable)
	void SwitchToLane(int Value);

	UFUNCTION(BlueprintCallable)
	AGroundTile* AddGroundTile();

	UFUNCTION(BlueprintCallable)
	void RecycleTile(AGroundTile* Tile);

protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;
};



