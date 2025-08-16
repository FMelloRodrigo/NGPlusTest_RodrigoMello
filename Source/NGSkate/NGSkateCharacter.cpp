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

	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_PhysicsBody, ECollisionResponse::ECR_Ignore);
	
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
	PhysicsBallMesh->SetCollisionObjectType(ECollisionChannel::ECC_PhysicsBody);

	SkateMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkateMesh"));
	SkateMesh->SetupAttachment(GetMesh(), TEXT("hand_r"));
	SkateMesh->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));
	SkateMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void ANGSkateCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (OnSkate)
	{
		IsSkateFallingCheck();
		SetMeshLocationAndRotation();
		CheckDirectionAngleBreak();
		ClampSpeed();
	}
	

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

		
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ANGSkateCharacter::SkateMoveInput);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &ANGSkateCharacter::EndMove);
		
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
void ANGSkateCharacter::EndMove()
{
	MovementVector = FVector2D::ZeroVector;
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

void ANGSkateCharacter::SkateMoveInput(const FInputActionValue& Value)
{
	MovementVector = Value.Get<FVector2D>();
	OnSkate ? ProcessSkateInput() : Move(Value);
}

void ANGSkateCharacter::EnterSkateInput()
{
	OnSkate ? ExitSkate() : EnterSkate();
}

void ANGSkateCharacter::JumpEvent()
{
	OnSkate ? SkateJumpImpulse() : Jump();
}

void ANGSkateCharacter::SkateJumpImpulse()
{
	if (PhysicsBallMesh->IsSimulatingPhysics())
	{
		float LocalJumpSpeed;

		// Small boost if in movement, to compesate
		HasMovementInput() ? LocalJumpSpeed = (JumpStrength * 1.5f) : LocalJumpSpeed = JumpStrength;

		PhysicsBallMesh->AddImpulse(FVector(0.f, 0.f, LocalJumpSpeed));
	}
}

void ANGSkateCharacter::ProcessSkateInput()
{
	if(OnSkate)
	{
		if (MovementVector.Y == 1)
		{
			CalculateSpeedDirection();
			Accelerate(12);
		}
		if (MovementVector.Y == -1)
		{
			Break(45);
		}
		if (MovementVector.X != 0)
		{
			ProcessLateralInput(5);
		}
	}
}
void ANGSkateCharacter::ProcessLateralInput(float Strength)
{
	CalculateSpeedDirection();
	if (GetPhysicsVelocity() > 500.f)
	{	
		Accelerate(5);
	}

}

void ANGSkateCharacter::IsSkateFallingCheck()
{
	FHitResult HitResult;
	FVector Start = PhysicsBallMesh->GetComponentLocation();
	FVector End = Start + (GetActorUpVector() * (-80.f) );

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(this);


	IsFallingOnSkate = !GetWorld()->LineTraceSingleByObjectType(HitResult,Start,End,FCollisionObjectQueryParams(ObjectTypes),TraceParams);
}

void ANGSkateCharacter::SetMeshLocationAndRotation()
{
	FVector Min;
	FVector Max;
	PhysicsBallMesh->GetLocalBounds(Min, Max);

	const FVector BallOffset = FVector(0.f, 0.f, GetCapsuleComponent()->GetScaledCapsuleHalfHeight() - Max.Z);
	const FVector BallLocation = PhysicsBallMesh->GetComponentLocation() + BallOffset;
	const FVector PhysVelocity = UKismetMathLibrary::Normal(( PhysicsBallMesh->GetPhysicsLinearVelocity()));
	const FRotator PhysRotation = FRotator( 0.f, UKismetMathLibrary::MakeRotFromX(PhysVelocity).Yaw, 0.f );

	SetActorLocationAndRotation(BallLocation, PhysRotation);
}

void ANGSkateCharacter::CheckDirectionAngleBreak()
{
	const FRotator CalcRot = FRotator(0.f, GetControlRotation().Yaw,0.f );
	const FVector CalcDir = UKismetMathLibrary::GetForwardVector(CalcRot);
	
	const float AngleCalc = UKismetMathLibrary::Dot_VectorVector(CurrentMovementInput, CalcDir);
	
	if (AngleCalc <= (-0.75f))
	{
		Break(40);
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

	else if (!HasMovementInput() && !IsFallingOnSkate)
	
	{
		Break(10);
	}
	
}

bool ANGSkateCharacter::HasMovementInput()
{
	return (MovementVector.X != 0 || MovementVector.Y != 0);
}

FVector ANGSkateCharacter::CalcMovementInput()
{
	const FRotator LocalRot = FRotator(0.f, GetControlRotation().Yaw, 0.f);
	const FVector FDir = UKismetMathLibrary::GetForwardVector(LocalRot);
	const FVector RDir = UKismetMathLibrary::GetRightVector(LocalRot);

	return (FDir * MovementVector.Y) + (RDir * MovementVector.X);

}

void ANGSkateCharacter::Break(float Strenght)
{
	const float BrakeAlpha = FMath::Clamp(Strenght / 100.0f, 0.0f, 1.0f);
	const float BrakePower = 5.0f;
	const float Decay = FMath::Clamp(1.0f - (BrakeAlpha * UGameplayStatics::GetWorldDeltaSeconds(this) * BrakePower), 0.0f, 1.0f);


	FVector PhysVel = PhysicsBallMesh->GetPhysicsLinearVelocity();

	PhysicsBallMesh->SetPhysicsLinearVelocity(PhysVel * Decay);

	//GEngine->AddOnScreenDebugMessage(-1, 0.1f, FColor::Yellow,FString::Printf(TEXT("Strength: %f | Decay: %f | Speed: %f"),Strenght, Decay, PhysVel.Size()));	
}

void ANGSkateCharacter::EnterSkate()
{
	if (!GetCharacterMovement()->IsFalling())
	{
		//const FAttachmentTransformRules& AttachmentRules(FAttachmentTransformRules::KeepRelativeTransform);

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
		PhysicsBallMesh->SetSimulatePhysics(true);
		SkateMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("root"));
		OnSkate = true;

	}
}

void ANGSkateCharacter::ExitSkate()
{
	if (!IsFallingOnSkate)
	{
		//const FAttachmentTransformRules& AttachmentRules(FAttachmentTransformRules::SnapToTargetIncludingScale);

		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		PhysicsBallMesh->SetSimulatePhysics(false);
		SkateMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, TEXT("hand_r"));
		PhysicsBallMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		OnSkate = false;

	}
}

float ANGSkateCharacter::GetPhysicsVelocity()
{
	return PhysicsBallMesh->GetPhysicsLinearVelocity().Size();
}
