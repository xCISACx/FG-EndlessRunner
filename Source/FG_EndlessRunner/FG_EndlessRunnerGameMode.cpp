// Copyright Epic Games, Inc. All Rights Reserved.

#include "FG_EndlessRunnerGameMode.h"

#include <string>

#include "CoreMinimal.h"
#include "BaseObstacle.h"
#include "EndlessRunnerSaveGame.h"
#include "FG_EndlessRunnerCharacter.h"
#include "GameOver.h"
#include "GroundTile.h"
#include "GroundTilesParent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "../../Plugins/Developer/RiderLink/Source/RD/thirdparty/clsocket/src/StatTimer.h"
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
	
	/*FTimerHandle TimerHandle1, TimerHandle2;

	//This needs to be done because the character won't recognise it's grounded unless the state of SimulatePhysics is reset.

	bool bValue = false;
	
	// Wait for 1 second, then toggle physics to true
	GetWorldTimerManager().SetTimer(TimerHandle1, [this, bValue]() { TogglePhysics(bValue); }, 0.2f, false);

	bValue = true;
	
	// Wait for another second, then toggle physics to false
	GetWorldTimerManager().SetTimer(TimerHandle2, [this, bValue]() { TogglePhysics(bValue); }, 0.3f, false);*/

	UWorld* World = GetWorld();

	InputManager = World->SpawnActor<AInputManager>(InputManagerClass, FVector(0, 0, 20), FRotator(0,0,0));

	WalkingPlane = World->SpawnActor<AWalkingPlane>(WalkingPlaneClass, WalkingPlaneSpawnLocation, FRotator(0,0,0));

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
	
	/*if (GroundTileClass)
	{
		LaneSwitchValues.Add(-255);
		LaneSwitchValues.Add(0);
		LaneSwitchValues.Add(255);	
	}*/

	CreateInitialGroundTiles();

	// Create an instance of the widget
	UUserWidget* MyWidget = CreateWidget<UUserWidget>(GetWorld(), UIBlueprint);

	UI = MyWidget;

	// Add the widget to the viewport
	UI->AddToViewport();

	UpdateScore(0, 0);
	UpdateScore(1, 0);
}

void AFG_EndlessRunnerGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsGameOver || IsPaused()) return;

	Speed += SpeedIncrease;
	UpdateScore(0, SpeedIncrease);
	UpdateScore(1, SpeedIncrease);
}

void AFG_EndlessRunnerGameMode::TogglePhysics(bool Value)
{
	bool bIsSimulatingPhysics = Player->GetCapsuleComponent()->IsSimulatingPhysics();
	FString PhysicsState = bIsSimulatingPhysics ? TEXT("true") : TEXT("false");
	
	Player->GetCapsuleComponent()->SetSimulatePhysics(Value);
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, FString::Printf(TEXT("IsSimulatingPhysics: %s"), *PhysicsState));
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

	//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Black, "Chosen Tile: " + GroundTiles[RandInt]->GetName());

	const int TileObstacleCount = GroundTiles[RandInt]->Obstacles.Num() - 1;

	//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Black,"Original Tile Obstacle Count: " + TileObstacleCount);

	const int RandomObstacleIndex = FMath::RandRange(0, TileObstacleCount - 1);

	ABaseObstacle* ObstacleToDestroy;
	
	if (GroundTiles[RandInt]->Obstacles.Num() <= 1)
	{
		return;
	}
	
	ObstacleToDestroy = GroundTiles[RandInt]->Obstacles[RandomObstacleIndex];

	/*GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Black,
			"Removed: " + GroundTiles[RandInt]->Obstacles[RandomObstacleIndex]->GetName() + " from tile: " + GroundTiles[RandInt]->GetName());

	GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Black, "Had " + FString::FromInt(TileObstacleCount + 1) + " has " + FString::FromInt(GroundTiles[RandInt]->Obstacles.Num() - 1));*/

	GroundTiles[RandInt]->Obstacles.RemoveAt(RandomObstacleIndex);

	ObstacleToDestroy->Destroy();
}

void AFG_EndlessRunnerGameMode::RecycleTile(AGroundTile* Tile)
{
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "Recycling: " + Tile->GetName());
	ClearObstacles(Tile);
	Tile->AnyObstacleHit = false;
	Tile->GroundTriggerBox->SetGenerateOverlapEvents(true);
	
	int32 lastIndex = GroundTiles.Num() - 1;
	Tile->SetActorLocation(GroundTiles[lastIndex]->AttachPoint->GetComponentLocation());
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, GroundTiles[lastIndex]->AttachPoint->GetComponentLocation().ToString());
	GroundTiles.Remove(Tile);
	GroundTiles.Add(Tile);

	SpawnObstacles(Tile, 0);
	SpawnObstacles(Tile, 1);
	SpawnObstacles(Tile, 2);
}

void AFG_EndlessRunnerGameMode::GameOver()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->bShowMouseCursor = true;
	
	IsGameOver = true;

	if(GameOverClass)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
		
		UUserWidget* Widget = CreateWidget(GetWorld(), GameOverClass);

		if(Widget)
		{
			Widget->AddToViewport();

			auto WinningPlayer = ScoreP1 > ScoreP2 ? 1 : 2;
			
			Cast<UGameOver>(Widget)->PlayerWinText->SetText(FText::FromString("Player " + FString::FromInt(WinningPlayer) + " Wins!"));
			Cast<UGameOver>(Widget)->ScoreText->SetText(FText::FromString("Score: " + FString::FromInt(FMath::Max(ScoreP1, ScoreP2))));
		}
	}
	
	UEndlessRunnerSaveGame* SaveGame = UEndlessRunnerSaveGame::LoadScore();

	if (ScoreP1 || ScoreP2 > SaveGame->HighScore)
	{
		SaveGame->SaveScore(FMath::Max(ScoreP1, ScoreP2));	
	}
	
	//UGameplayStatics::SetGamePaused(GetWorld(), true);
}

void AFG_EndlessRunnerGameMode::RestartLevel()
{
	if (!IsGameOver) return;
	
	// Get the current level's name
	FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this, true);

	// Open the level again to reset it
	UGameplayStatics::OpenLevel(this, *CurrentLevelName);

	IsGameOver = false;

	UGameplayStatics::SetGamePaused(GetWorld(), false);
}

void AFG_EndlessRunnerGameMode::UpdateLives(int PlayerID, int Value)
{
	const UOverlay* LivesOverlay;
	UCanvasPanelSlot* LivesImageSlot;
	
	switch (PlayerID)
	{
	case 0:
		LivesP1 = FMath::Max(LivesP1 + Value, 0);

		LivesOverlay = Cast<UOverlay>(UI->GetWidgetFromName(TEXT("LivesOverlay1")));
		//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Orange, LivesOverlay->GetName());
		LivesImageSlot = Cast<UCanvasPanelSlot>(LivesOverlay->Slot);
		//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Orange, LivesImageSlot->GetName());
		//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Orange, FString::FromInt(LivesImageSlot->GetSize().X));
		LivesImageSlot->SetSize(FVector2D(LivesP1 * 50, LivesImageSlot->GetSize().Y));
		//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Orange, FString::FromInt(LivesImageSlot->GetSize().X));
		break;
		
	case 1:
		LivesP2 = FMath::Max(LivesP2 + Value, 0);

		LivesOverlay = Cast<UOverlay>(UI->GetWidgetFromName(TEXT("LivesOverlay2")));
		//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Orange, LivesOverlay->GetName());
		LivesImageSlot = Cast<UCanvasPanelSlot>(LivesOverlay->Slot);
		//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Orange, LivesImageSlot->GetName());
		//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Orange, FString::FromInt(LivesImageSlot->GetSize().X));
		LivesImageSlot->SetSize(FVector2D(LivesP2 * 50, LivesImageSlot->GetSize().Y));
		//GEngine->AddOnScreenDebugMessage(-1, 6.0f, FColor::Orange, FString::FromInt(LivesImageSlot->GetSize().X));
		break;
	}

	if (LivesP1 <= 0 || LivesP2 <= 0)
	{
		GameOver();
	}
}

void AFG_EndlessRunnerGameMode::UpdateScore(int PlayerID, float Value)
{
	UTextBlock* ScoreTextBlock;
	
	switch (PlayerID)
	{
	case 0:
		ScoreP1 = FMath::Max(ScoreP1 + Value, 0);
		ScoreTextBlock = Cast<UTextBlock>(UI->GetWidgetFromName(TEXT("ScoreText1")));
		ScoreTextBlock->SetText(FText::FromString(FString::FromInt(ScoreP1)));
		break;
		
	case 1:
		ScoreP2 = FMath::Max(ScoreP2 + Value, 0);
		ScoreTextBlock = Cast<UTextBlock>(UI->GetWidgetFromName(TEXT("ScoreText2")));
		ScoreTextBlock->SetText(FText::FromString(FString::FromInt(ScoreP2)));
		break;
	}
}

void AFG_EndlessRunnerGameMode::TogglePlayerInvincibility(int PlayerID, bool Value)
{
	if (Player)
	{
		AFG_EndlessRunnerCharacter* PlayerReference = Cast<AFG_EndlessRunnerCharacter>(InputManager->Characters[PlayerID]);
		PlayerReference->bIsInvincible = Value;

		FTimerHandle DestroyTimerHandle;

		GetWorldTimerManager().SetTimer(DestroyTimerHandle, [this, Value, PlayerReference]() {
			PlayerReference->bIsInvincible = !Value;
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

	//GEngine->AddOnScreenDebugMessage(-1, 20.0f, FColor::Yellow, "Clearing obstacles from " + Tile->GetName());
}

void AFG_EndlessRunnerGameMode::PauseGame()
{
	if (PauseMenuClass)
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	
		UUserWidget* Widget = CreateWidget(GetWorld(), PauseMenuClass);

		if(Widget)
		{
			Widget->AddToViewport();
		}

		APlayerController* PlayerControllerRef = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		PlayerControllerRef->SetShowMouseCursor(true);
	}
}


