// Copyright Epic Games, Inc. All Rights Reserved.

#include "FG_EndlessRunnerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GroundTile.h"
#include "FG_EndlessRunnerGameMode.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"


//////////////////////////////////////////////////////////////////////////
// AFG_EndlessRunnerCharacter

AFG_EndlessRunnerCharacter::AFG_EndlessRunnerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

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
	// Call the base class  
	Super::BeginPlay();

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

//////////////////////////////////////////////////////////////////////////
// Input

void AFG_EndlessRunnerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "Bound jump");

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFG_EndlessRunnerCharacter::Move);

		//Looking
		//EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFG_EndlessRunnerCharacter::Look);

	}

}

void AFG_EndlessRunnerCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		//AddMovementInput(ForwardDirection, MovementVector.Y);
		//AddMovementInput(RightDirection, MovementVector.X);

		if (GameMode->CanSwitchLanes)
		{
			int sign = FMath::Sign(MovementVector.X);
            
            GameMode->CurrentLaneIndex += (int) sign;
            
            GameMode->CurrentLaneIndex = FMath::Clamp(GameMode->CurrentLaneIndex, 0, 2);
    
            GameMode->SwitchToLane(GameMode->CurrentLaneIndex);
		}

		
	}
}

void AFG_EndlessRunnerCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AFG_EndlessRunnerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Get a reference to the character movement component
	UCharacterMovementComponent* MyCharacterMovement = GetCharacterMovement();

	// Print the values of the jumping bools to the screen
	if (MyCharacterMovement)
	{
		/*GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
			FString::Printf(TEXT("IsFalling: %s"), MyCharacterMovement->IsFalling() ? TEXT("True") : TEXT("False")));
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
			FString::Printf(TEXT("IsJumpingAllowed: %s"), MyCharacterMovement->IsJumpAllowed() ? TEXT("True") : TEXT("False")));*/
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
			FString::Printf(TEXT("CanJump: %s"), CanJump() ? TEXT("True") : TEXT("False")));
	}

	AFG_EndlessRunnerGameMode* MyGameMode = GetWorld()->GetAuthGameMode<AFG_EndlessRunnerGameMode>();

	FVector MovementDirection = FVector(1.f, 0.f, 0.f); // Move in X direction
	
	UPrimitiveComponent* PlayerComponent = Cast<UPrimitiveComponent>(GetRootComponent());
	
	if (PlayerComponent)
	{
		FVector GroundForce = MovementDirection * GameMode->Speed * DeltaSeconds;
		FVector Velocity = MovementDirection * GameMode->Speed;
		PlayerComponent->SetPhysicsLinearVelocity(Velocity, false);
		//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "Adding force to ground");
	}
	
	// Get a reference to the movement component
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();

	// Print the current movement mode
	UE_LOG(LogTemp, Warning, TEXT("Current movement mode: %d"), MovementComponent->MovementMode.GetValue());

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow,
			FString::Printf(TEXT("Pressed Jump: %s"), ACharacter::bPressedJump ? TEXT("True") : TEXT("False")));
}

void AFG_EndlessRunnerCharacter::Jump()
{
	if (CanJump())
	{
		Super::Jump();
		GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Blue, "JUMP");
	}
}


void AFG_EndlessRunnerCharacter::OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, "AAAAAAAAAAAAAAAAAAAAA");
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, OtherActor->GetName());
	//GEngine->AddOnScreenDebugMessage(-1, 99.0f, FColor::Yellow, OtherComp->GetName());

	OtherActor->GetClass()

	if (OtherComp->ComponentHasTag("GroundTriggerBox"))
	{
		AGroundTile* Tile = Cast<AGroundTile>(OtherActor);

		if (Tile)
		{
			GameMode->RecycleTile(Tile);
		}
	}
}




