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

	/*// Create a new sphere component
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Box Component"));

	// Set its collision properties
	BoxComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// Set its size and location
	BoxComponent->SetSphereRadius(50.f);
	BoxComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

	// Attach it to the actor's root component
	BoxComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Enable physics simulation for the component
	BoxComponent->SetSimulatePhysics(true);
	
	RootComponent = BoxComponent;*/
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

