// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "SkateCore/IScoreActor.h"
#include "Components/BoxComponent.h"
#include "NGSkatePlayerState.h"
#include "NGSkatePlayerController.h"
#include "NGSkateCharacter.generated.h"

class USpringArmComponent;
class UBoxComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
struct FAttachmentTransformRules;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class ANGSkateCharacter : public ACharacter
{
	GENERATED_BODY()

	

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* EnterSkateAction;

	//Skate Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SkateComponents, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PhysicsBallMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SkateComponents, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* SkateMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SkateComponents, meta = (AllowPrivateAccess = "true"))
	UBoxComponent* JumpOverlapComponent;

public:
	
	ANGSkateCharacter();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Skate)
	bool OnSkate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Skate)
	bool IsFallingOnSkate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input)
	FVector2D MovementVector;

// Options
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Options)
	float JumpStrength = 40.f;

	
	

protected:

	
	void Move(const FInputActionValue& Value);
	void EndMove();
	
	void Look(const FInputActionValue& Value);

// Skate Functions
	void SkateMoveInput(const FInputActionValue& Value);
	void EnterSkateInput();
	void EnterSkate();
	void ExitSkate();
	void JumpEvent();
	void SkateJumpImpulse();
	void Accelerate(float Strenght);
	void Break(float Strenght);

// Scoring

	void CheckforJumpObstacles();

	UFUNCTION()
	void OnJumpOverlapEnd(UPrimitiveComponent* OverlappedComp,AActor* OtherActor,UPrimitiveComponent* OtherComp,int32 OtherBodyIndex);

// Skate Variables

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Skate)
	FVector WantedMovementInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Skate)
	FVector CurrentMovementInput;
			

protected:

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	

	void ProcessSkateInput();
	void ProcessLateralInput(float Strength);
	float GetPhysicsVelocity();
	void IsSkateFallingCheck();
	void SetMeshLocationAndRotation();
	void CheckDirectionAngleBreak();
	void CalculateSpeedDirection();
	void ClampSpeed();
	bool HasMovementInput();
	FVector CalcMovementInput();


public:
	
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

