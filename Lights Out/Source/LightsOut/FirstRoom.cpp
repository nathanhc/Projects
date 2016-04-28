// Fill out your copyright notice in the Description page of Project Settings.

#include "LightsOut.h"
#include "FirstRoom.h"
#include "SoundGem.h"
#include "LightsOutCharacter.h"
#include "Flashlight.h"
#include "Sound/SoundCue.h"

void AFirstRoom::BeginPlay()
{
	Super::BeginPlay();
    
	CurrentGoal = 0;
    IsSolved = false;
    HasFailed = false;
}

void AFirstRoom::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

//Correct Sequence is in Blue, Green, Purple, Red
bool AFirstRoom::CheckSequence(ASoundGem *LitGem)
{
    if(IsSolved)
    {
        return true;
    }
    
	if (LitGem->GetLightColor() == SoundGems[CurrentGoal]->GetLightColor())
    {
		CurrentGoal++;
        HasFailed = false;
		if (CurrentGoal >= SoundGems.Num())
		{
			OnCompletePuzzle();
		}
        return true;
	}
	else
    {
		OnFailPuzzle();
        return false;
	}
}

void AFirstRoom::SpawnPuzzle()
{
	Super::SpawnPuzzle();
}

void AFirstRoom::OnCompletePuzzle()
{
	Super::OnCompletePuzzle();
    
    SoundGems[0]->PlayWinAudio();
    IsSolved = true;
	SolvedAudioComponent = PlaySound(DoorSound);
    if(Door)
    {
         Door->Destroy();   
    }
    if(Character)
    {
        Character->GetFlashlight()->AddBatteryTime(20);
    }
}

void AFirstRoom::OnFailPuzzle()
{
	Super::OnFailPuzzle();
    
	CurrentGoal = 0;
	for (int i = 0; i < SoundGems.Num(); i++)
    {
		SoundGems[i]->Reset();
	}
    if(!HasFailed)
    {
        SoundGems[0]->PlayFailAudio();
        HasFailed = true;
    }
}

bool AFirstRoom::CheckIsSolved()
{
    return false;
}

UAudioComponent *AFirstRoom::PlaySound(USoundCue *Sound)
{
	UAudioComponent *AC = nullptr;
	if (Sound)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, RootComponent);
	}
	return AC;
}

