// Fill out your copyright notice in the Description page of Project Settings.

#include "LightsOut.h"
#include "SoundGem.h"
#include "Sound/SoundCue.h"
#include "FirstRoom.h"


ASoundGem::ASoundGem()
{
    PrimaryActorTick.bCanEverTick = true;
    
    PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
    SetRootComponent(PointLightComponent);

	LightColor = FColor(255, 255, 255, 255);
}

void ASoundGem::BeginPlay()
{
    Super::BeginPlay();
	mCurrIntensity = mDefaultIntensity;
	LerpSpeed = 1000.0f;
	m_IsShining = false;
	PointLightComponent->Intensity = mDefaultIntensity;
	PointLightComponent->SetLightColor(LightColor, true);
}

void ASoundGem::Tick(float DeltaTime)
{
    Super::Tick( DeltaTime );
}

void ASoundGem::RespondToFlashlightHit()
{
	if (!m_IsShining)
	{
		LightUp();
	}
}

void ASoundGem::LightUp() 
{
	if (firstroom && firstroom->CheckSequence(this))
	{
		PointLightComponent->SetIntensity(LightIntensity);
		m_IsShining = true;
		GemAudioComponent = PlaySound(pitch);
	}
}

void ASoundGem::OnShine() 
{
	mCurrIntensity += LerpSpeed;
	mCurrIntensity = FMath::Clamp(mCurrIntensity, mDefaultIntensity, LightIntensity);
	PointLightComponent->Intensity = mCurrIntensity;
}

bool ASoundGem::IsSolved() 
{
	return m_IsShining;
}

void ASoundGem::PlayFailAudio()
{
    GemAudioComponent = PlaySound(Fail);
}

void ASoundGem::PlayWinAudio()
{
    GemAudioComponent = PlaySound(Win);
}

void ASoundGem::Reset()
{
	PointLightComponent->SetIntensity(0);
    m_IsShining = false;
}

UAudioComponent *ASoundGem::PlaySound(USoundCue *Sound)
{
	UAudioComponent *AC = nullptr;
	if (Sound)
	{
		AC = UGameplayStatics::SpawnSoundAttached(Sound, RootComponent);
	}
	return AC;
}

