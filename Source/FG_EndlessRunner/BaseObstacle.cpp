// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseObstacle.h"

#include "FG_EndlessRunnerGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseObstacle::ABaseObstacle()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionComponent"));

	RootComponent = CollisionComponent;

	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionObjectType(ECC_WorldStatic);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

}

// Called when the game starts or when spawned
void ABaseObstacle::BeginPlay()
{
	Super::BeginPlay();

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseObstacle::OnCollisionBoxBeginOverlap);

	GameMode = Cast<AFG_EndlessRunnerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	check(GameMode);
}

// Called every frame
void ABaseObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseObstacle::OnCollisionBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this && OtherActor == GetOwner()) return;

	if (OtherActor->ActorHasTag("Player"))
	{
		AFG_EndlessRunnerCharacter* Player = Cast<AFG_EndlessRunnerCharacter>(OtherActor);
		
		if (GameMode != nullptr)
		{
			if (!Player->bIsInvincible)
			{
				GameMode->UpdateLives(Player->PlayerID, -1);
				GameMode->UpdateScore(Player->PlayerID, -ScoreDecrease);
				WasHit = true;
			}
			GameMode->TogglePlayerInvincibility(Player->PlayerID, true);
			this->Destroy();
		}
	}
}

