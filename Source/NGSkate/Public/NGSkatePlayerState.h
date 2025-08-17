// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "NGSkatePlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnScoreChanged, int32, NewScore, int32, CurrentScore, FName, ActionName);

UCLASS()
class NGSKATE_API ANGSkatePlayerState : public APlayerState
{
	GENERATED_BODY()


public:

	void ReportScore(int32 InScore, FName InName);

	UPROPERTY(BlueprintAssignable)
	FOnScoreChanged OnScoreChanged;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = ScoreSystem)
	int32 CurrentPlayerPoints;
	
};
