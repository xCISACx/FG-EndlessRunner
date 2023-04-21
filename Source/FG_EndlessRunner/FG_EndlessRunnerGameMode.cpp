// Copyright Epic Games, Inc. All Rights Reserved.

#include "FG_EndlessRunnerGameMode.h"
#include "CoreMinimal.h"
#include "FG_EndlessRunnerCharacter.h"
#include "GroundTile.h"
#include "GroundTilesParent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

#include "Kismet/GameplayStatics.h"


AFG_EndlessRunnerGameMode::AFG_EndlessRunnerGameMode()
{
	// set default pawn class to our Blueprinted character
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

	// Cast the player character to your specific player class
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
	
	/*UWorld* World = GetWorld();

	if (World)
	{
		GroundTilesParent = World->SpawnActor<AActor>(AActor::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator);
		check(GroundTilesParent);

		// Create a new root component for the actor
		USceneComponent* MyRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
		GroundTilesParent->SetRootComponent(MyRootComponent);
		MyRootComponent->SetMobility(EComponentMobility::Movable);
		check(MyRootComponent);

		/*UPrimitiveComponent* RootComponent = Cast<UPrimitiveComponent>(GroundTilesParent->GetRootComponent());
		if (RootComponent)
		{
			RootComponent->SetSimulatePhysics(false);
			RootComponent->SetCollisionProfileName(TEXT("OverlapAll"));
		}#1#
	}*/

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
	


	/*if (Player && GroundTilesParent)
	{
		FVector PlayerLocation = Player->GetRootComponent()->GetComponentLocation();
		FVector GroundLocation = GroundTilesParent->GetRootComponent()->GetComponentLocation();

		PlayerLocation += MovementDirection * Speed * DeltaSeconds;
		GroundLocation -= MovementDirection * Speed * DeltaSeconds;

		Player->SetActorLocation(PlayerLocation);
		GroundTilesParent->SetActorLocation(GroundLocation);
	}*/

	//if (GroundTilesParent)
	//{
	//	FVector DeltaLocation = FVector(Speed * DeltaSeconds, 0.f, 0.f);

	//	GroundTilesParent->AddActorLocalOffset(DeltaLocation);

	//	// Calculate the movement of the player relative to the platform
	//	FVector PlayerDeltaLocation = DeltaLocation.RotateAngleAxis(-Player->GetActorRotation().Yaw, FVector::UpVector);

	//	// Move the player relative to the platform
	//	Player->AddActorWorldOffset(PlayerDeltaLocation);

	//	//// Get the current location of the actor
	//	//FVector ActorLocation = GroundTilesParent->GetActorLocation();

	//	//// Get the forward vector of the actor
	//	//FVector ForwardVector = GroundTilesParent->GetActorForwardVector();

	//	//// Calculate the new location of the actor
	//	//FVector NewLocation = ActorLocation + (-ForwardVector * Speed * DeltaSeconds);

	//	//// Set the new location of the actor
	//	//GroundTilesParent->SetActorLocation(NewLocation);
	//}

	//// Calculate the desired movement direction
	//FVector MovementDirection = GroundTilesParent->GetActorForwardVector();

	//// Normalize the direction vector to get a unit vector
	//MovementDirection.Normalize();

	//// Calculate the force magnitude based on desired speed
	//float ForceMagnitude = DesiredSpeed / GetWorld()->GetDefaultGravityZ();

	//// Calculate the force vector
	//FVector Force = MovementDirection;

	//UPrimitiveComponent* ParentRootComponent = Cast<UPrimitiveComponent>(GroundTilesParent->GetRootComponent());

	//// Add the force to the actor
	//ParentRootComponent->AddForce(Force, NAME_None, true);
}

void AFG_EndlessRunnerGameMode::CreateInitialGroundTiles()
{
	AGroundTile* Tile = AddGroundTile();

	if (Tile)
	{
		LaneSwitchValues.Add(Tile->LeftLane->GetComponentLocation().Y);
		LaneSwitchValues.Add(Tile->CenterLane->GetComponentLocation().Y);
		LaneSwitchValues.Add(Tile->RightLane->GetComponentLocation().Y);	
	}
	
	for (int i = 0; i < NumInitialGroundTiles; i++)
	{
		AddGroundTile();
	}
}

void AFG_EndlessRunnerGameMode::SwitchToLane(int LaneIndex)
{
	float TargetY = LaneSwitchValues[LaneIndex];
    
	if (Player && CanSwitchLanes)
	{
		CanSwitchLanes = false;
	}
	
	/*float TargetY = LaneSwitchValues[Value];
	
	float LerpSpeed = 0.5f;
	
	if (Player && CanSwitchLanes)
	{
		FVector CurrentPlayerPos = Player->GetActorLocation();
		
		float NewY = FMath::Lerp(CurrentPlayerPos.Y, TargetY, LerpSpeed);
		
		Player->SetActorLocation(FVector(CurrentPlayerPos.X, NewY, CurrentPlayerPos.Z));

		FTimerHandle TimerHandle;

		// Start the timer to repeat this function until the actor reaches the target Y position
		GetWorldTimerManager().SetTimer(TimerHandle, [this, Value]() {
			SwitchToLane(Value);
		}, 1.f, true);

		CanSwitchLanes = true;
	}*/

	/*if (Plane)
	{
		FVector CurrentPlanePos = Plane->GetActorLocation();

		// Lerp the Y position to the target Y position
		float NewPlaneY = FMath::Lerp(CurrentPlanePos.Y, TargetY, LerpSpeed);
	
		Plane->SetActorLocation(FVector(CurrentPlanePos.X, NewPlaneY, CurrentPlanePos.Z));
	}*/
}

AGroundTile* AFG_EndlessRunnerGameMode::AddGroundTile()
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

void AFG_EndlessRunnerGameMode::RecycleTile(AGroundTile* Tile)
{
	int32 lastIndex = GroundTiles.Num() - 1;
	Tile->SetActorLocation(GroundTiles[lastIndex]->AttachPoint->GetComponentLocation());
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, GroundTiles[lastIndex]->GetName());
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, GroundTiles[lastIndex]->AttachPoint->GetComponentLocation().ToString());
	GroundTiles.Remove(Tile);
	GroundTiles.Add(Tile);
}


