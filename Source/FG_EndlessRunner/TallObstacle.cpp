// Fill out your copyright notice in the Description page of Project Settings.


#include "TallObstacle.h"

void ATallObstacle::OnCollisionBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnCollisionBoxOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "Hit Tall Obstacle");
}

