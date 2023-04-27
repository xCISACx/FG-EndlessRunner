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
#include "Blueprint/UserWidget.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerStart.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


class UTextBlock;
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

	// Spawn player 2
	APlayerStart* PlayerStart2 = Cast<APlayerStart>(FindPlayerStart(2), APlayerStart::StaticClass()));
	ACharacter* Player2 = GetWorld()->SpawnActor<ACharacter>(DefaultPawnClass, PlayerStart2->GetActorLocation(), PlayerStart2->GetActorRotation());
	APlayerController* PlayerController2 = GetWorld()->SpawnActor<APlayerController>(PlayerControllerClass);
	PlayerController2->Possess(Player2);
	
	/*FTimerHandle TimerHandle1, TimerHandle2;

	//This needs to be done because the character won't recognise it's grounded unless the state of SimulatePhysics is reset.

	bool bValue = false;
	
	// Wait for 1 second, then toggle physics to true
	GetWorldTimerManager().SetTimer(TimerHandle1, [this, bValue]() { TogglePhysics(bValue); }, 0.2f, false);

	bValue = true;
	
	// Wait for another second, then toggle physics to false
	GetWorldTimerManager().SetTimer(TimerHandle2, [this, bValue]() { TogglePhysics(bValue); }, 0.3f, false);*/

	UWorld* World = GetWorld();

	AWalkingPlane* NewWalkingPlane = World->SpawnActor<AWalkingPlane>(WalkingPlaneClass, FVector(0, 0, 20), FRotator(0,0,0));

	WalkingPlane = NewWalkingPlane;

	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	
	AFG_EndlessRunnerCharacter* MyPlayer = Cast<AFG_EndlessRunnerCharacter>(PlayerCharacter);

	if (MyPlayer != nullptr)
	{
		// Assign the player to the player property
		Player = MyPlayer;
		/*Player->GetCapsuleComponent()->SetSimulatePhysics(false);
		Player->GetCapsuleComponent()->SetSimulatePhysics(true);*/
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

	// Create an instance of the widget
	UUserWidget* MyWidget = CreateWidget<UUserWidget>(GetWorld(), UIBlueprint);

	UI = MyWidget;

	// Add the widget to the viewport
	UI->AddToViewport();

	UpdateScore(0);
}

void AFG_EndlessRunnerGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	Speed += SpeedIncrease;
	UpdateScore(SpeedIncrease);

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

void AFG_EndlessRunnerGameMode::TogglePhysics(bool Value)
{
	bool bIsSimulatingPhysics = Player->GetCapsuleComponent()->IsSimulatingPhysics();
	FString PhysicsState = bIsSimulatingPhysics ? TEXT("true") : TEXT("false");
	
	Player->GetCapsuleComponent()->SetSimulatePhysics(Value);
	GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, FString::Printf(TEXT("IsSimulatingPhysics: %s"), *PhysicsState));
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
			const float RandValType = FMath::FRandRange(0.f, 1.f);

			ABaseObstacle* Obstacle;
			
			if (UKismetMathLibrary::InRange_FloatFloat(RandValType, 0.5f, 1.f, true, true))
			{
				Obstacle = GetWorld()->SpawnActor<ABaseObstacle>(ShortObstacleClass, SpawnLocation, SpawnParameters);
				Obstacle->AttachToComponent(Lane, FAttachmentTransformRules::KeepRelativeTransform);
				Obstacle->SetActorRelativeLocation(FVector(0.f, 0.f, 0));
			}
			else
			{
				Obstacle = GetWorld()->SpawnActor<ABaseObstacle>(TallObstacleClass, SpawnLocation, SpawnParameters);
				Obstacle->AttachToComponent(Lane, FAttachmentTransformRules::KeepRelativeTransform);
				Obstacle->SetActorRelativeLocation(FVector(0.f, 0.f, TallObstacleZOffset));
			}
			
			Tile->Obstacles.Add(Obstacle);

			//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "Spawning obstacle on : " + Tile->GetName());
		}
	}
}

void AFG_EndlessRunnerGameMode::RemoveRandomObstacle()
{
	const int RandInt = FMath::RandRange(1, GroundTiles.Num() - 1);

	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Black,
			"Chosen Tile: " + GroundTiles[RandInt]->GetName());

	const int TileObstacleCount = GroundTiles[RandInt]->Obstacles.Num() - 1;

	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Black,
			"Original Tile Obstacle Count: " + TileObstacleCount);

	const int RandomObstacleIndex = FMath::RandRange(0, TileObstacleCount - 1);

	ABaseObstacle* ObstacleToDestroy;
	
	if (GroundTiles[RandInt]->Obstacles.Num() <= 0)
	{
		return;
	}
	else
	{
		ObstacleToDestroy = GroundTiles[RandInt]->Obstacles[RandomObstacleIndex];
	}

	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Black,
			"Removed: " + GroundTiles[RandInt]->Obstacles[RandomObstacleIndex]->GetName() + " from tile: " + GroundTiles[RandInt]->GetName());

	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Black, "Had " + FString::FromInt(TileObstacleCount + 1) + " has " + FString::FromInt(GroundTiles[RandInt]->Obstacles.Num() - 1));

	GroundTiles[RandInt]->Obstacles.RemoveAt(RandomObstacleIndex);

	ObstacleToDestroy->Destroy();
}

void AFG_EndlessRunnerGameMode::RecycleTile(AGroundTile* Tile)
{
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "Recycling: " + Tile->GetName());
	ClearObstacles(Tile);
	Tile->AnyObstacleHit = false;
	
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
	Lives = FMath::Max(Lives + Value, 0);

	if (Lives <= 0)
	{
		// Get the current level's name
		FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);

		// Open the level again to reset it
		UGameplayStatics::OpenLevel(this, *CurrentLevelName);
	}
	
	const UOverlay* LivesOverlay = Cast<UOverlay>(UI->GetWidgetFromName(TEXT("LivesOverlay")));
	GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Orange, LivesOverlay->GetName());
	UCanvasPanelSlot* LivesImageSlot = Cast<UCanvasPanelSlot>(LivesOverlay->Slot);
	GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Orange, LivesImageSlot->GetName());
	GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Orange, FString::FromInt(LivesImageSlot->GetSize().X));
	LivesImageSlot->SetSize(FVector2D(Lives * 50, LivesImageSlot->GetSize().Y));
	GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Orange, FString::FromInt(LivesImageSlot->GetSize().X));
}

void AFG_EndlessRunnerGameMode::UpdateScore(float Value)
{
	Score = FMath::Max(Score + Value, 0);
	
	UTextBlock* ScoreTextBlock = Cast<UTextBlock>(UI->GetWidgetFromName(TEXT("ScoreText")));
	ScoreTextBlock->SetText(FText::FromString(FString::FromInt(Score)));
}

void AFG_EndlessRunnerGameMode::TogglePlayerInvincibility(bool Value)
{
	if (Player)
	{
		Player->bIsInvincible = Value;

		FTimerHandle DestroyTimerHandle;

		GetWorldTimerManager().SetTimer(DestroyTimerHandle, [this, Value]() {
			Player->bIsInvincible = !Value;
		}, InvincibilityDuration, false);
	}
}

void AFG_EndlessRunnerGameMode::ClearObstacles(AGroundTile* Tile)
{
	for (int32 i = Tile->Obstacles.Num() - 1; i >= 0; i--)
	{
		ABaseObstacle* Obstacle = Tile->Obstacles[i];
		
		if (Obstacle)
		{
			Tile->Obstacles.RemoveAt(i);
			Obstacle->Destroy();
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, "Clearing obstacles from " + Tile->GetName());
}


