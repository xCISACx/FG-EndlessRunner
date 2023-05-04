// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GroundTile.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Components/BoxComponent.h"
#include "FG_EndlessRunnerCharacter.generated.h"


UCLASS(config=Game)
class AFG_EndlessRunnerCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UBoxComponent* TriggerBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
	class UCapsuleComponent* CollisionCapsule;

public:
	AFG_EndlessRunnerCharacter();

	/*/** Jump Input Action #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* JumpAction;

	/** Move Input Action #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* MoveAction;

	/** Look Input Action #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	/** Slide Input Action #1#
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* SlideAction;*/

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	int PlayerID = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	bool bIsInvincible = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Collision)
	bool bIsSliding = false;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	bool CanSwitchLanes = true;

	UPROPERTY(EditAnywhere, Category = "Runtime")
	bool IsSwitchingLanes = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	float DefaultCapsuleHalfHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	float DefaultCapsuleRadius;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	FVector DefaultCapsuleLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Config)
	float SlideDuration = 0.5f;

	virtual void Jump() override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	void ResetLaneSwitch();

	void Slide();

protected:

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	virtual void Tick(float DeltaSeconds) override;
	
	UFUNCTION(BlueprintCallable)
	void OnTriggerBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	void OnCollisionBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                           UPrimitiveComponent* OtherComp,
	                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleInstanceOnly)
	class AFG_EndlessRunnerGameMode* GameMode;
			

protected:
	void ResetCapsuleSize();
	void ResetSlideState();
	void StopSliding();
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

