// Fill out your copyright notice in the Description page of Project Settings.


#include "NGSkatePlayerState.h"

void ANGSkatePlayerState::ReportScore(int32 InScore, FName InName)
{
	
	CurrentPlayerPoints += InScore;

	OnScoreChanged.Broadcast(InScore, CurrentPlayerPoints, InName);

}