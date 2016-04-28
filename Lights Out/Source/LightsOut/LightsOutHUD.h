// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.
#pragma once 
#include "GameFramework/HUD.h"
#include "LightsOutHUD.generated.h"

UCLASS()
class ALightsOutHUD : public AHUD
{
	GENERATED_BODY()

public:
	ALightsOutHUD();

	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;
//    void SetVisible(bool vis){ this->
private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

};

