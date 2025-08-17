// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NGSkate/NGSkateCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NGSkateAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class NGSKATE_API UNGSkateAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

    virtual void NativeInitializeAnimation() override;

    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bOnSkate;
 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bWantsToJump;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation")
    bool bSkateAccelerating;

private:

    ANGSkateCharacter* OwningPawn;
	
};
