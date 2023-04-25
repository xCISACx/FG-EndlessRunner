// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseObstacle.h"
#include "TallObstacle.generated.h"

/**
 * 
 */
UCLASS()
class FG_ENDLESSRUNNER_API ATallObstacle : public ABaseObstacle
{
	GENERATED_BODY()

public:
	
	virtual void OnCollisionBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	
};
