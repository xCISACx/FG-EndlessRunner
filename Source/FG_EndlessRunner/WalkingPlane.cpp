// Fill out your copyright notice in the Description page of Project Settings.


#include "WalkingPlane.h"

// Sets default values
AWalkingPlane::AWalkingPlane()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;	

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	MeshComponent->SetStaticMesh(LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Plane.Plane")));

	RootComponent = MeshComponent;
}

// Called when the game starts or when spawned
void AWalkingPlane::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWalkingPlane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

