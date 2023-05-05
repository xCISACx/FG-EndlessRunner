// Copyright Epic Games, Inc. All Rights Reserved.

#include "FG_EndlessRunnerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FG_EndlessRunnerGameMode.h"
#include "GroundTile.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"


//////////////////////////////////////////////////////////////////////////
// AFG_EndlessRunnerCharacter

AFG_EndlessRunnerCharacter::AFG_EndlessRunnerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCapsuleComponent()->OnComponentBeginOverlap.AddDynamic(this, &AFG_EndlessRunnerCharacter::OnCollisionBoxOverlap);
	
	CollisionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionCapsule"));
	CollisionCapsule->InitCapsuleSize(42.f, 96.f);
	CollisionCapsule->SetupAttachment(RootComponent);
	CollisionCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionCapsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetBoxExtent(FVector(32.0f, 32.0f, 88.0f));
	TriggerBox->SetupAttachment(RootComponent);
	TriggerBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TriggerBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AFG_EndlessRunnerCharacter::OnTriggerBoxOverlap);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 10000.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 350.0f; // The camera follows at this distance behind the character	
	CameraBoom->SocketOffset = FVector(0.f, 0.f, 100.f);
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AFG_EndlessRunnerCharacter::BeginPlay()
{
	Super::BeginPlay();

	GameMode = Cast<AFG_EndlessRunnerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	check(GameMode);

	DefaultCapsuleHalfHeight = CollisionCapsule->GetUnscaledCapsuleHalfHeight();
	DefaultCapsuleRadius = CollisionCapsule->GetUnscaledCapsuleRadius();
}



void AFG_EndlessRunnerCharacter::Slide()
{
	if (GameMode->IsGameOver) return;
	
	float SlideCapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float SlideCapsuleRadius = GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	// Reduce the size of the capsule
	CollisionCapsule->SetCapsuleHalfHeight(SlideCapsuleHalfHeight / 3.f, false);
	CollisionCapsule->SetCapsuleRadius(SlideCapsuleRadius);
	CollisionCapsule->SetRelativeLocation(FVector(0, 0, -SlideCapsuleHalfHeight /2.f));

	bIsSliding = true;
	
	FTimerHandle SlideTimerHandle;

	// Set a timer to reset the capsule size after the slide duration
	GetWorldTimerManager().SetTimer(SlideTimerHandle, this, &AFG_EndlessRunnerCharacter::ResetCapsuleSize, SlideDuration, false);
}

void AFG_EndlessRunnerCharacter::ResetCapsuleSize()
 {
 	// Reset the size of the capsule
 	CollisionCapsule->SetCapsuleHalfHeight(DefaultCapsuleHalfHeight, false);
 	CollisionCapsule->SetCapsuleRadius(DefaultCapsuleRadius);
	CollisionCapsule->SetRelativeLocation(FVector(0,0,0));
 
 	bIsSliding = false;
 }

void AFG_EndlessRunnerCharacter::StopSliding()
{
	ResetCapsuleSize();
}

void AFG_EndlessRunnerCharacter::Move(const FInputActionValue& Value)
{
	if (GameMode->IsGameOver) return;
	
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (CanSwitchLanes)
	{
		int sign = FMath::Sign(MovementVector.X);
		
		switch (PlayerID)
		{
			case 0:
				GameMode->CurrentLaneIndexP1 += (int) sign;
	            
				GameMode->CurrentLaneIndexP1 = FMath::Clamp(GameMode->CurrentLaneIndexP1, 0, 2);

				CanSwitchLanes = false;
			
				IsSwitchingLanes = true;
				break;

			case 1:
				sign = FMath::Sign(MovementVector.X);
		            
				GameMode->CurrentLaneIndexP2 += (int) sign;
		            
				GameMode->CurrentLaneIndexP2 = FMath::Clamp(GameMode->CurrentLaneIndexP2, 0, 2);

				CanSwitchLanes = false;
				
				IsSwitchingLanes = true;
				break;
				
			default: ;
		}
	}
}

void AFG_EndlessRunnerCharacter::ResetLaneSwitch()
{
	if (!CanSwitchLanes)
	{
		CanSwitchLanes = true;
		IsSwitchingLanes = false;
	}
}

void AFG_EndlessRunnerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFG_EndlessRunnerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (IsSwitchingLanes)
	{
		FVector CurrentPos = GetActorLocation();
		
		float NewY = 0;
		
		switch (PlayerID)
		{
			case 0:
				NewY = GameMode->LaneSwitchValues[GameMode->CurrentLaneIndexP1];
				break;

			case 1:	
				NewY = GameMode->LaneSwitchValues[GameMode->CurrentLaneIndexP2];
				break;
				
			default: ;
		}
		
		FVector LerpTarget = FVector(CurrentPos.X, NewY, CurrentPos.Z);
		FVector NewPos = FMath::Lerp(CurrentPos, LerpTarget, DeltaSeconds * GameMode->LanesSwitchingSpeed);
		SetActorLocation(NewPos);

		if (FVector::DistSquared(CurrentPos, LerpTarget) < .3f)
		{
			ResetLaneSwitch();
			SetActorLocation(LerpTarget);
		}
	}
}

void AFG_EndlessRunnerCharacter::Jump()
{
	if (GameMode->IsGameOver) return;
	
	GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Blue, "JUMP");
	Super::Jump();
}


void AFG_EndlessRunnerCharacter::OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag("GroundTriggerBox"))
	{
		OtherComp->SetGenerateOverlapEvents(false);
		AGroundTile* Tile = Cast<AGroundTile>(OtherActor);

		if (Tile)
		{
			GameMode->RecycleTile(Tile);
		}
	}
}

void AFG_EndlessRunnerCharacter::OnCollisionBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherComp->ComponentHasTag("GroundTriggerBox"))
	{
		AGroundTile* Tile = Cast<AGroundTile>(OtherActor);

		if (Tile && Tile->Obstacles.Num() != 0)
		{
			for (ABaseObstacle* Obstacle : Tile->Obstacles)
			{
				if (Obstacle && Obstacle->WasHit)
				{
					Tile->AnyObstacleHit = true;
				}
			}
				
			if (Tile->Obstacles.Num() > 1 && !Tile->AnyObstacleHit)
			{
				GameMode->UpdateScore(PlayerID, +GameMode->ScoreIncrease);
					
				const int RandVal = FMath::RandRange(0, 1);

				if (RandVal <= 0.25)
				{
					GameMode->RemoveRandomObstacle();
				}
			}
		}
	}
}




