// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NGSkateCore/IScoreActor.h"
#include "JumpObstacle.generated.h"

UCLASS()
class NGSKATE_API AJumpObstacle : public AActor, public IIScoreActor
{
	GENERATED_BODY()
	
public:	

	AJumpObstacle();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = SkateComponents)
	UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Scoring")
	int32 ScoreValue = 10;



	virtual int32 GetScoreValue_Implementation() const override { return ScoreValue; }





};
