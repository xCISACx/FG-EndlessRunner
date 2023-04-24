// Copyright Epic Games, Inc. All Rights Reserved.

#include "FG_EndlessRunnerGameMode.h"

#include <string>

#include "CoreMinimal.h"
#include "BaseObstacle.h"
#include "FG_EndlessRunnerCharacter.h"
#include "GroundTile.h"
#include "GroundTilesParent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


class AUIActor;

AFG_EndlessRunnerGameMode::AFG_EndlessRunnerGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	PrimaryActorTick.bCanEverTick = true;
}
void AFG_EndlessRunnerGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();

	AWalkingPlane* NewWalkingPlane = World->SpawnActor<AWalkingPlane>(WalkingPlaneClass, FVector(0, 0, 20), FRotator(0,0,0));

	WalkingPlane = NewWalkingPlane;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	Plane = FindObject<AStaticMeshActor>(GetWorld(), TEXT("Plane"));
	
	AFG_EndlessRunnerCharacter* MyPlayer = Cast<AFG_EndlessRunnerCharacter>(PlayerCharacter);

	if (MyPlayer != nullptr)
	{
		// Assign the player to the player property
		Player = MyPlayer;
		Player->GetCapsuleComponent()->SetSimulatePhysics(false);
		Player->GetCapsuleComponent()->SetSimulatePhysics(true);
	}

	AGroundTilesParent* NewGroundTilesParent = World->SpawnActor<AGroundTilesParent>(GroundTilesParentClass, NextSpawnPoint);

	GroundTilesParent = NewGroundTilesParent;

	/*AUIActor* UIActor = GetWorld()->SpawnActor<AUIActor>(AUIActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, FActorSpawnParameters());

	UUserWidget* UI = CreateWidget<UUserWidget>(UIActor, UIWidgetClass);

	if (UI)
	{
		LivesText = Cast<UTextBlock>(UI->GetWidgetFromName(TEXT("LivesText")));
		
		if (LivesText)
		{
			LivesText->SetText(FText::FromString("Lives: " + FString::FromInt(Lives)));
		}
	}*/
	
	if (GroundTileClass)
	{
		LaneSwitchValues.Add(-255);
		LaneSwitchValues.Add(0);
		LaneSwitchValues.Add(255);	
	}

	CreateInitialGroundTiles();
}

void AFG_EndlessRunnerGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Speed += SpeedIncrease;

	if (Player && !CanSwitchLanes)
	{
		FVector CurrentPos = Player->GetActorLocation();
		float NewY = LaneSwitchValues[CurrentLaneIndex];
		FVector LerpTarget = FVector(CurrentPos.X, NewY, CurrentPos.Z);
		FVector NewPos = FMath::Lerp(CurrentPos, LerpTarget, DeltaSeconds * LanesSwitchingSpeed);
		Player->SetActorLocation(NewPos);

		if (FVector::DistSquared(CurrentPos, LerpTarget) < .3f)
		{
			CanSwitchLanes = true;
			Player->SetActorLocation(LerpTarget);
		}
	}
}

void AFG_EndlessRunnerGameMode::CreateInitialGroundTiles()
{
	AGroundTile* Tile = SpawnGroundTile();

	if (Tile)
	{
		LaneSwitchValues.Add(Tile->LeftLane->GetComponentLocation().Y);
		LaneSwitchValues.Add(Tile->CenterLane->GetComponentLocation().Y);
		LaneSwitchValues.Add(Tile->RightLane->GetComponentLocation().Y);    
	}

	for (int i = 0; i < NumInitialGroundTiles; i++)
	{
		AGroundTile* SpawnedTile = SpawnGroundTile();
        
		SpawnObstacles(SpawnedTile, 0);
		SpawnObstacles(SpawnedTile, 1);
		SpawnObstacles(SpawnedTile, 2);
	}
}

void AFG_EndlessRunnerGameMode::SwitchToLane(int LaneIndex)
{
	float TargetY = LaneSwitchValues[LaneIndex];
    
	if (Player && CanSwitchLanes)
	{
		CanSwitchLanes = false;
	}
}

AGroundTile* AFG_EndlessRunnerGameMode::SpawnGroundTile()
{
	UWorld* World = GetWorld();

	if (World)
	{
		AGroundTile* Tile = World->SpawnActor<AGroundTile>(GroundTileClass, NextSpawnPoint);

		if (Tile)
		{
			NextSpawnPoint = Tile->AttachPoint->GetComponentTransform();
		}

		Tile->AttachToComponent(GroundTilesParent->GetRootComponent(), FAttachmentTransformRules::KeepWorldTransform);
		//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "Parented " + Tile->GetName() + " to ground tiles parent.");

		GroundTiles.Add(Tile);

		return Tile;
	}

	return nullptr;
}

void AFG_EndlessRunnerGameMode::SpawnObstacles(AGroundTile* Tile, int laneIndex)
{
	UArrowComponent* Lane = nullptr;

	if (laneIndex == 0)
	{
		Lane = Tile->LeftLane;
	}
	else if (laneIndex == 1)
	{
		Lane = Tile->CenterLane;
	}
	else if (laneIndex == 2)
	{
		Lane = Tile->RightLane;
	}

	if (Lane)
	{
		const float RandVal = FMath::FRandRange(0.f, 1.f);
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		const FTransform& SpawnLocation = Lane->GetComponentTransform();

		if (UKismetMathLibrary::InRange_FloatFloat(RandVal, 0.5f, 1.f, true, true))
		{
			ABaseObstacle* Obstacle = GetWorld()->SpawnActor<ABaseObstacle>(SmallObstacleClass, SpawnLocation, SpawnParameters);
			Obstacle->AttachToComponent(Lane, FAttachmentTransformRules::KeepRelativeTransform);
			Obstacle->SetActorRelativeLocation(FVector(0.f, 0.f, 0));
			Tile->Obstacles.Add(Obstacle);

			//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "Spawning obstacle on : " + Tile->GetName());
		}
	}
}

void AFG_EndlessRunnerGameMode::RecycleTile(AGroundTile* Tile)
{
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "Recycling: " + Tile->GetName());
	ClearObstacles(Tile);
	
	int32 lastIndex = GroundTiles.Num() - 1;
	Tile->SetActorLocation(GroundTiles[lastIndex]->AttachPoint->GetComponentLocation());
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, GroundTiles[lastIndex]->AttachPoint->GetComponentLocation().ToString());
	GroundTiles.Remove(Tile);
	GroundTiles.Add(Tile);

	SpawnObstacles(Tile, 0);
	SpawnObstacles(Tile, 1);
	SpawnObstacles(Tile, 2);
}

void AFG_EndlessRunnerGameMode::UpdateLives(int Value)
{
	Lives += Value;
	//LivesText->SetText(FText::FromString("Lives: " + Lives));
}

void AFG_EndlessRunnerGameMode::TogglePlayerInvincibility(bool Value, float Duration)
{
	if (Player)
	{
		Player->bIsInvincible = Value;

		FTimerHandle DestroyTimerHandle;

		GetWorldTimerManager().SetTimer(DestroyTimerHandle, [this, Value]() {
			Player->bIsInvincible = !Value;
		}, Duration, false);
	}
}

void AFG_EndlessRunnerGameMode::ClearObstacles(AGroundTile* Tile)
{
	for (ABaseObstacle* Obstacle : Tile->Obstacles)
	{
		if (Obstacle)
		{
			GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "Clearing obstacles from " + Tile->GetName());
			Tile->Obstacles.Remove(Obstacle);
			Obstacle->Destroy();
		}
	}
}


