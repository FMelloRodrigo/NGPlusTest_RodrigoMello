// Copyright Epic Games, Inc. All Rights Reserved.

#include "NGSkateCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "InputActionValue.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

ANGSkateCharacter::ANGSkateCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	
	GetCharacterMovement()->bOrientRotationToMovement = true; 	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); 

	
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 700.0f; 
	CameraBoom->bUsePawnControlRotation = true; 

	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); 
	FollowCamera->bUsePawnControlRotation = false; 

	PhysicsBallMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PhysicsBallMesh"));
	PhysicsBallMesh->SetupAttachment(RootComponent);
	PhysicsBallMesh->SetVisibility(false);

	SkateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkateMesh"));
	SkateMesh->SetupAttachment(GetMesh(), TEXT("hand_r"));
	SkateMesh->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

}

void ANGSkateCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ProcessSkateInput();
	CheckDirectionAngleBreak();
	ClampSpeed();
}



void ANGSkateCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ANGSkateCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ANGSkateCharacter::JumpEvent);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANGSkateCharacter::Move);

		
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ANGSkateCharacter::Look);

		EnhancedInputComponent->BindAction(EnterSkateAction, ETriggerEvent::Started, this, &ANGSkateCharacter::EnterSkateInput);

	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}



void ANGSkateCharacter::Move(const FInputActionValue& Value)
{
	
	MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void ANGSkateCharacter::Look(const FInputActionValue& Value)
{
	
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ANGSkateCharacter::EnterSkateInput()
{


}

void ANGSkateCharacter::JumpEvent()
{
	OnSkate ? SkateJumpImpulse() : Jump();
}

void ANGSkateCharacter::SkateJumpImpulse()
{
	if (PhysicsBallMesh->IsSimulatingPhysics())
	{
		PhysicsBallMesh->AddImpulse(FVector(0.f, 0.f, 40000.f));
	}
}

void ANGSkateCharacter::ProcessSkateInput()
{
	if(OnSkate)
	{
		IsSkateFallingCheck();
		SetMeshLocationAndRotation();
	}
}

void ANGSkateCharacter::IsSkateFallingCheck()
{
	FHitResult HitResult;
	FVector Start = GetActorLocation();
	FVector End = Start + (GetActorUpVector() * 100.f);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);


	FallingOnSkateFalling = GetWorld()->LineTraceSingleByObjectType(HitResult,Start,End,FCollisionObjectQueryParams(ObjectTypes),TraceParams);
}

void ANGSkateCharacter::SetMeshLocationAndRotation()
{
	FVector Min;
	FVector Max;
	PhysicsBallMesh->GetLocalBounds(Min, Max);

	const FVector BallOffset = FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() + Max.Z);
	const FVector BallLocation = PhysicsBallMesh->GetComponentLocation() + BallOffset;
	const FVector PhysVelocity = UKismetMathLibrary::Normal(( PhysicsBallMesh->GetPhysicsLinearVelocity()));
	const FRotator PhysRotation = FRotator( 0.f, 0.f, UKismetMathLibrary::MakeRotFromX(PhysVelocity).Yaw);

	SetActorLocationAndRotation(BallLocation, PhysRotation);
}

void ANGSkateCharacter::CheckDirectionAngleBreak()
{
	const FRotator CalcRot = FRotator(0.f, 0.f, GetControlRotation().Yaw);
	const FVector CalcDir = UKismetMathLibrary::GetForwardVector(CalcRot);
	
	const float AngleCalc = UKismetMathLibrary::Dot_VectorVector(CurrentMovementInput, CalcDir);
	
	if (AngleCalc <= (-0.75f))
	{
		Break(100);
	}
}

void ANGSkateCharacter::CalculateSpeedDirection()
{
	WantedMovementInput = CalcMovementInput();
	CurrentMovementInput = UKismetMathLibrary::VInterpTo(CurrentMovementInput, WantedMovementInput, UGameplayStatics::GetWorldDeltaSeconds(this), 10.f);

}

void ANGSkateCharacter::Accelerate(float Strenght)
{
	const FVector PushVelocity = UKismetMathLibrary::Normal(CurrentMovementInput) * (Strenght * 20000.f);
	PhysicsBallMesh->AddForce(PushVelocity);

}

void ANGSkateCharacter::ClampSpeed()
{
	const FVector PhysDir = UKismetMathLibrary::Normal(PhysicsBallMesh->GetPhysicsLinearVelocity());
	const float PhysSpeed = PhysicsBallMesh->GetPhysicsLinearVelocity().Size();
	
	const FVector ClampedVelocity = PhysDir * 1000.f;
	
	if (PhysSpeed > 1000.f)
	{
		PhysicsBallMesh->SetPhysicsLinearVelocity(ClampedVelocity);
	}

	else
	{
		if (!HasMovementInput && !FallingOnSkateFalling)
		{
			Break(10);
		}
	}

}

bool ANGSkateCharacter::HasMovementInput()
{
	return (MovementVector.X != 0 || MovementVector.Y != 0);
}

FVector ANGSkateCharacter::CalcMovementInput()
{
	const FRotator LocalRot = FRotator(0.f, 0.f, GetControlRotation().Yaw);
	const FVector FDir = UKismetMathLibrary::GetForwardVector(LocalRot);
	const FVector RDir = UKismetMathLibrary::GetRightVector(LocalRot);

	return (FDir * MovementVector.X) + (RDir * MovementVector.Y);

}

void ANGSkateCharacter::Break(float Strenght)
{
	const float BreakStrength =( 100 - Strenght) * UGameplayStatics::GetWorldDeltaSeconds(this);
	const float ClampedBreakStrength = UKismetMathLibrary::Clamp(BreakStrength, 0.1f, 0.99f);
	const FVector PhysDir = PhysicsBallMesh->GetPhysicsLinearVelocity();

	PhysicsBallMesh->SetPhysicsLinearVelocity(PhysDir * ClampedBreakStrength);

}