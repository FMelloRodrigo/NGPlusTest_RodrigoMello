// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IScoreActor.generated.h"


UINTERFACE(MinimalAPI)
class UIScoreActor : public UInterface
{
	GENERATED_BODY()
};


class NGSKATE_API IIScoreActor
{
	GENERATED_BODY()

	
public:

	
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Scoring")
	int32 GetScoreValue() const;

	virtual int32 GetScoreValue_Implementation() const
	{
		return 10; 
	}
	
};
