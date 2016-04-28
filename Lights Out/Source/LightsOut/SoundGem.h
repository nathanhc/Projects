// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HittableObject.h"
#include "SoundGem.generated.h"

UCLASS()
class LIGHTSOUT_API ASoundGem : public AHittableObject
{
	GENERATED_BODY()
	
    public:
        ASoundGem();
        void BeginPlay() override;
        void Tick( float DeltaSeconds ) override;
		void RespondToFlashlightHit() override;
		void LightUp();
		void OnShine();
		bool IsSolved();
		void Reset();
        void PlayFailAudio();
        void PlayWinAudio();
        FColor GetLightColor(){ return LightColor;}
		class UAudioComponent* PlaySound(class USoundCue *Sound);

    protected:
		UPROPERTY(Transient)
		class UAudioComponent *GemAudioComponent;

        UPROPERTY(EditDefaultsOnly, Category = Sound)
        class USoundCue* pitch;
    
        UPROPERTY(EditDefaultsOnly, Category = Sound)
        class USoundCue *Fail;
    
        UPROPERTY(EditDefaultsOnly, Category = Sound)
        class USoundCue *Win;
    
        UPROPERTY(EditDefaultsOnly, Category = Light)
        UPointLightComponent *PointLightComponent;
        
        UPROPERTY(EditDefaultsOnly, Category = Light)
        FColor LightColor;
        
        UPROPERTY(EditDefaultsOnly, Category = Light)
        float LightIntensity = 5000.0f;
       
        UPROPERTY(EditDefaultsOnly, Category = Light)
        float LerpSpeed;

		UPROPERTY(EditDefaultsOnly, Category = Light)
		float mDefaultIntensity = 0.0f;

		UPROPERTY(EditAnywhere)
		class AFirstRoom* firstroom;

		float TimerRate = 0.1f;

		float mCurrIntensity;

		bool m_IsShining;

		FTimerHandle IntensityTimer;
	
};
