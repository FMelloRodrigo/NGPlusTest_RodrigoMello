// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "NGSkatePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NGSKATE_API ANGSkatePlayerController : public APlayerController
{
	GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UUserWidget> ScoreWidgetClass;


    UPROPERTY()
    UUserWidget* ScoreWidgetInstance;
   
protected:

    virtual void BeginPlay() override;
    

   



	
};
