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

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseObstacle::OnCollisionBoxOverlap);

	GameMode = Cast<AFG_EndlessRunnerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	check(GameMode);
}

// Called every frame
void ABaseObstacle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseObstacle::OnCollisionBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == this && OtherActor == GetOwner()) return;

	if (OtherActor->ActorHasTag("Player"))
	{
		AFG_EndlessRunnerCharacter* Player;
		Player = Cast<AFG_EndlessRunnerCharacter>(OtherActor);
		
		GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "hit player");
		
		if (GameMode != nullptr)
		{
			if (!Player->bIsInvincible)
			{
				GameMode->UpdateLives(-1);
				GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "updating lives");	
			}
			GameMode->TogglePlayerInvincibility(true);
			this->Destroy();
		}
	}
}

