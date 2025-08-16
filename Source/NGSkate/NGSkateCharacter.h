// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Components/StaticMeshComponent.h"
#include "NGSkateCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

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

public:
	
	ANGSkateCharacter();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Skate)
	bool OnSkate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Skate)
	bool FallingOnSkateFalling;

	FVector2D MovementVector;
	

protected:

	
	void Move(const FInputActionValue& Value);

	
	void Look(const FInputActionValue& Value);

// Skate Functions

	void EnterSkateInput();
	void JumpEvent();
	void SkateJumpImpulse();
	void Accelerate(float Strenght);
	void Break(float Strenght);



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

