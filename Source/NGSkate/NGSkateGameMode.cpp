// Copyright Epic Games, Inc. All Rights Reserved.

#include "NGSkateGameMode.h"
#include "NGSkateCharacter.h"
#include "UObject/ConstructorHelpers.h"

ANGSkateGameMode::ANGSkateGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
