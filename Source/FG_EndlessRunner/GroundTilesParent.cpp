// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundTilesParent.h"
#include "Kismet/GameplayStatics.h"
#include "FG_EndlessRunnerGameMode.h"
#include "Components/SphereComponent.h"

// Sets default values
AGroundTilesParent::AGroundTilesParent()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));

	// Create a new sphere component
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere Component"));

	// Set its collision properties
	SphereComponent->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	// Set its size and location
	SphereComponent->SetSphereRadius(50.f);
	SphereComponent->SetRelativeLocation(FVector(0.f, 0.f, 50.f));

	// Attach it to the actor's root component
	SphereComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	// Enable physics simulation for the component
	SphereComponent->SetSimulatePhysics(true);
	
	RootComponent = SphereComponent;
}

// Called when the game starts or when spawned
void AGroundTilesParent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AGroundTilesParent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AFG_EndlessRunnerGameMode* GameMode = GetWorld()->GetAuthGameMode<AFG_EndlessRunnerGameMode>();

	FVector MovementDirection = FVector(1.f, 0.f, 0.f); // Move in X direction

	UPrimitiveComponent* PrimitiveRootComponent = Cast<UPrimitiveComponent>(RootComponent);
	
	if (PrimitiveRootComponent)
	{
		FVector GroundForce = MovementDirection * GameMode->Speed * DeltaTime;
		FVector Velocity = MovementDirection * GameMode->Speed;
		//SetActorLocation(GetActorLocation() - Velocity / 600);
		PrimitiveRootComponent->SetPhysicsLinearVelocity(-Velocity, false);
		//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "Adding force to ground");
	}
}

