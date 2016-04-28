// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LightsOut.h"
#include "LightsOutGameMode.h"
#include "LightsOutHUD.h"
#include "LightsOutCharacter.h"

ALightsOutGameMode::ALightsOutGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ALightsOutHUD::StaticClass();
}
