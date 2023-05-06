// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseObstacle.h"
#include "GameFramework/Actor.h"
#include "Components/ArrowComponent.h" 
#include "GroundTile.generated.h"

class USceneComponent;
class UStaticMeshComponent;
class UArrowComponent;
class UBoxComponent;

UCLASS()
class FG_ENDLESSRUNNER_API AGroundTile : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* GroundMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* AttachPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* CenterLane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* LeftLane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UArrowComponent* RightLane;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	float CenterLaneY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	float LeftLaneY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	float RightLaneY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UBoxComponent* GroundTriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	bool AnyObstacleHit;

	UPROPERTY(VisibleInstanceOnly, Category = "Runtime")
	TArray<ABaseObstacle*> Obstacles;

	DECLARE_EVENT(AGroundTile, FOnTileOverlap);
	FOnTileOverlap OnTileOverlap;

	// Sets default values for this actor's properties
	AGroundTile();

protected:

	UPROPERTY(VisibleInstanceOnly)
	class AFG_EndlessRunnerGameMode* GameMode;

	UFUNCTION()
	void OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
