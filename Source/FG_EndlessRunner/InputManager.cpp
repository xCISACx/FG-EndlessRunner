// Fill out your copyright notice in the Description page of Project Settings.


#include "InputManager.h"

#include "BaseEnemy.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "FG_EndlessRunnerGameMode.h"
#include "Kismet/GameplayStatics.h"

class UEnhancedInputLocalPlayerSubsystem;
// Sets default values
AInputManager::AInputManager()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AInputManager::BeginPlay()
{
	Super::BeginPlay();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), CharacterClass, Characters);

	Player1 = Cast<AFG_EndlessRunnerCharacter>(Characters[0]);
	Player1->PlayerID = 0;
	Player2 = Cast<AFG_EndlessRunnerCharacter>(Characters[1]);
	Player2->PlayerID = 1;

	check(Player1 && Player2)

	GameMode = Cast<AFG_EndlessRunnerGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	check(GameMode);

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
}

// Called every frame
void AInputManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AInputManager::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/*PlayerInputComponent->BindAxis("XAxisP1", this, &AInputManager::MoveP1);
	PlayerInputComponent->BindAction("JumpP1", EInputEvent::IE_Pressed, this, &AInputManager::JumpP1);
	PlayerInputComponent->BindAction("SlideP1", EInputEvent::IE_Pressed, this, &AInputManager::SlideP1);
		
	PlayerInputComponent->BindAxis("XAxisP2", this, &AInputManager::MoveP2);
	PlayerInputComponent->BindAction("JumpP2", EInputEvent::IE_Pressed, this, &AInputManager::JumpP2);
	PlayerInputComponent->BindAction("SlideP2", EInputEvent::IE_Pressed, this, &AInputManager::SlideP2);*/
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Replay
		EnhancedInputComponent->BindAction(ReplayAction, ETriggerEvent::Started, this, &AInputManager::Replay);

		//Pause
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &AInputManager::Pause);
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AInputManager::JumpP1);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AInputManager::StopJumpingP1);

		//Jumping
		EnhancedInputComponent->BindAction(JumpAction2, ETriggerEvent::Started, this, &AInputManager::JumpP2);
		EnhancedInputComponent->BindAction(JumpAction2, ETriggerEvent::Completed, this, &AInputManager::StopJumpingP2);

		//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "Bound jump");

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AInputManager::MoveP1);

		EnhancedInputComponent->BindAction(MoveAction2, ETriggerEvent::Triggered, this, &AInputManager::MoveP2);

		//Sliding
		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this, &AInputManager::SlideP1);

		EnhancedInputComponent->BindAction(SlideAction2, ETriggerEvent::Started, this, &AInputManager::SlideP2);

		GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "bound inputs");

	}

}

void AInputManager::MoveP1(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "MoveP1");
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Cyan, FString::FromInt(Value.Get<FVector2D>().X));
	Player1->Move(Value);
}

void AInputManager::MoveP2(const FInputActionValue& Value)
{
	GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "MoveP2");
	GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Cyan, FString::FromInt(Value.Get<FVector2D>().X));
	Player2->Move(Value);
}

void AInputManager::JumpP1()
{
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "JumpP1");
	Player1->Jump();
}

void AInputManager::JumpP2()
{
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "JumpP2");
	Player2->Jump();
}

void AInputManager::StopJumpingP1()
{
	Player1->Super::StopJumping();
}

void AInputManager::StopJumpingP2()
{
	Player2->Super::StopJumping();
}

void AInputManager::SlideP1()
{
	Player1->Slide();
}

void AInputManager::SlideP2()
{
	Player2->Slide();
}

void AInputManager::Replay()
{
	GameMode->RestartLevel();
}

void AInputManager::Pause()
{
	GameMode->PauseGame();
}

