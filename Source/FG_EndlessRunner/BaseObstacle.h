// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseObstacle.generated.h"

UCLASS()
class FG_ENDLESSRUNNER_API ABaseObstacle : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseObstacle();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleInstanceOnly)
	class AFG_EndlessRunnerGameMode* GameMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	class UBoxComponent* CollisionComponent;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	bool WasHit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	int ScoreDecrease;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	virtual void OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
