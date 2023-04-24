// Fill out your copyright notice in the Description page of Project Settings.


#include "GroundTile.h"

#include "FG_EndlessRunnerCharacter.h"
#include "FG_EndlessRunnerGameMode.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

class AFG_EndlessRunnerCharacter;

// Sets default values
AGroundTile::AGroundTile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	RootComponent = SceneComponent;

	AttachPoint = Cast<UArrowComponent>(GetDefaultSubobjectByName(TEXT("Attach Point")));

	GroundMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GroundMesh"));
	GroundMesh->SetupAttachment(SceneComponent);

	AttachPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("Attach Point"));
	AttachPoint->SetupAttachment(SceneComponent);

	CenterLane = CreateDefaultSubobject<UArrowComponent>(TEXT("Center Lane"));
	CenterLane->SetupAttachment(SceneComponent);

	LeftLane = CreateDefaultSubobject<UArrowComponent>(TEXT("Left Lane"));
	LeftLane->SetupAttachment(SceneComponent);

	RightLane = CreateDefaultSubobject<UArrowComponent>(TEXT("Right Lane"));
	RightLane->SetupAttachment(SceneComponent);

	GroundTriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("GroundTriggerBox"));
	GroundTriggerBox->SetupAttachment(SceneComponent);
	GroundTriggerBox->SetBoxExtent(FVector(16.f, 500.f, 160.f));
	GroundTriggerBox->SetCollisionProfileName("OverlapOnlyPawn");
}

void AGroundTile::OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AFG_EndlessRunnerCharacter* Character = Cast<AFG_EndlessRunnerCharacter>(OtherActor);

	if (!Character)
	{
		return;
	}

	// Check if the overlapping component is the GroundTriggerBox component of this tile
	if (OtherComp != GroundTriggerBox)
	{
		return;
	}

	OnTileOverlap.Broadcast();
}

// Called when the game starts or when spawned
void AGroundTile::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AFG_EndlessRunnerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	check(GameMode);

	GroundTriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AGroundTile::OnTriggerBoxOverlap);

	LeftLaneY = LeftLane->GetComponentLocation().Y;
	CenterLaneY = CenterLane->GetComponentLocation().Y;
	RightLaneY = RightLane->GetComponentLocation().Y;
}

 //Called every frame
void AGroundTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

