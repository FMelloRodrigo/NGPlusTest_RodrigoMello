// Fill out your copyright notice in the Description page of Project Settings.


#include "NGSkatePlayerState.h"

void ANGSkatePlayerState::ReportScore(int32 InScore, FName InName)
{
	GEngine->AddOnScreenDebugMessage(-88, 5.f, FColor::Green, FString::Printf(TEXT("Score: %d"), InScore));
	
	CurrentPlayerPoints += InScore;

	OnScoreChanged.Broadcast(InScore, CurrentPlayerPoints, InName);

}