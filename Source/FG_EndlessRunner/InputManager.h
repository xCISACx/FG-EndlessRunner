// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FG_EndlessRunnerCharacter.h"
#include "GameFramework/Pawn.h"
#include "InputManager.generated.h"

UCLASS()
class FG_ENDLESSRUNNER_API AInputManager : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AInputManager();

	UPROPERTY(VisibleInstanceOnly)
	class AFG_EndlessRunnerGameMode* GameMode;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AFG_EndlessRunnerCharacter> CharacterClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	TArray<AActor*> Characters;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	AFG_EndlessRunnerCharacter* Player1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Config)
	AFG_EndlessRunnerCharacter* Player2;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputMappingContext* DefaultMappingContext;

	/** Replay Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	class UInputAction* ReplayAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input1)
	class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input1)
	class UInputAction* MoveAction;

	/** Slide Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input1)
	class UInputAction* SlideAction;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input2)
	class UInputAction* JumpAction2;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input2)
	class UInputAction* MoveAction2;

	/** Slide Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input2)
	class UInputAction* SlideAction2;

	/** Pause Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	const UInputAction* PauseAction;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SlideP1();
	void SlideP2();
	void Replay();
	void Pause();
	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveP1(const FInputActionValue& Value);
	void MoveP2(const FInputActionValue& Value);

	UFUNCTION()
	void JumpP1();

	UFUNCTION()
	void JumpP2();
	void StopJumpingP1();
	void StopJumpingP2();
};
