// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FG_EndlessRunnerGameMode.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "GroundTilesParent.generated.h"

UCLASS()
class FG_ENDLESSRUNNER_API AGroundTilesParent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGroundTilesParent();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* SceneComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* SphereComponent;
	
	UPROPERTY(EditAnywhere, Category = "Config")
	TSubclassOf<AFG_EndlessRunnerGameMode> GameModeClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
