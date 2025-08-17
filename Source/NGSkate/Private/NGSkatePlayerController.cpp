// Fill out your copyright notice in the Description page of Project Settings.


#include "NGSkatePlayerController.h"




void ANGSkatePlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (ScoreWidgetClass) 
    {
        ScoreWidgetInstance = CreateWidget<UUserWidget>(this, ScoreWidgetClass);

        if (ScoreWidgetInstance)
        {
            ScoreWidgetInstance->AddToViewport();
        }
    }

}
