// Fill out your copyright notice in the Description page of Project Settings.


#include "TallObstacle.h"

void ATallObstacle::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnCollisionBoxBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (OtherActor == this && OtherActor == GetOwner()) return;

	if (OtherActor->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "Player Hit Tall Obstacle");
	}
}

