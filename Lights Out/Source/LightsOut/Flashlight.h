// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Flashlight.generated.h"

UCLASS()
class LIGHTSOUT_API AFlashlight : public AActor
{
	GENERATED_BODY()
	
    public:
    
        AFlashlight();
        virtual void BeginPlay() override;
        virtual void Tick(float DeltaSeconds) override;

        USkeletalMeshComponent *GetFlashlightMesh() { return FlashlightMesh; }
        void SetFlashlightMesh(USkeletalMeshComponent *NewMesh) { FlashlightMesh = NewMesh; }
    
        UFUNCTION(BlueprintCallable, BlueprintPure, Category="ParentClass")
        class ALightsOutCharacter *GetMyOwner() { return MyOwner; }
        void SetMyOwner(class ALightsOutCharacter *NewOwner) { MyOwner = NewOwner; }
    
        float GetBatteryTime() { return BatteryLife; }
        void AddBatteryTime(float Time)
        {
            BatteryLife += Time;
            if(BatteryLife > MaxBatteryLife)
            {
                BatteryLife = MaxBatteryLife;
            }
        }
    
        void ToggleLight();
    
        void SetLerp(float Value) { LerpDirection = Value; }
    
    protected:
    
        void Initialize();
        void LerpLight(float Percentage);
        void LerpConsumptionRate(float Percentage);
        void LerpRadius(float Percentage);
        void LerpIntensity(float Percentage);
        void LerpRange(float Percentage);
        void CastLight();
        class UAudioComponent *PlaySound(class USoundCue *Sound);
    
    protected:
    
        UPROPERTY(EditDefaultsOnly, Category = Components)
        USpotLightComponent *SpotLightComponent;
        UPROPERTY(VisibleDefaultsOnly, Category = Components)
        USkeletalMeshComponent *FlashlightMesh;
        UPROPERTY(Transient)
        class UAudioComponent *FlashlightAudioComponent;
    
        UPROPERTY(EditDefaultsOnly, Category = Rate)
        float LerpSpeed = 7.5f;
        UPROPERTY(EditDefaultsOnly, Category = Rate)
        float InitialPercentage = 0;
    
        UPROPERTY(EditDefaultsOnly, Category = Color)
        FColor LightColor;
    
        UPROPERTY(EditDefaultsOnly, Category = Sound)
        class USoundCue *ToggleOnSound;
        UPROPERTY(EditDefaultsOnly, Category = Sound)
        class USoundCue *ToggleOffSound;
    
        //Get the maximum battery life which can be changed by blueprint
        UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Battery)
        float MaxBatteryLife = 300.0f;
        //Get the current battery life but cannot by be changed by blueprint
        UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Battery)
        float BatteryLife = 300.0f;
        UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Battery)
        float MinimumConsumptionRate = 1.0f;
        UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Battery)
        float MaximumConsumptionRate = 5.0f;
    
        UPROPERTY(EditDefaultsOnly, Category = Radius)
        float MinimumRadius = 10.0f;
        UPROPERTY(EditDefaultsOnly, Category = Radius)
        float MaximumRadius = 40.0f;
        UPROPERTY(EditDefaultsOnly, Category = Radius)
        float InnerOuterConeRatio = 1.5;
    
        UPROPERTY(EditDefaultsOnly, Category = Intensity)
        float MinimumIntensity = 1000.0f;
        UPROPERTY(EditDefaultsOnly, Category = Intensity)
        float MaximumIntensity = 10000.0f;
    
        UPROPERTY(EditDefaultsOnly, Category = Range)
        float MinimumRange = 100.0f;
        UPROPERTY(EditDefaultsOnly, Category = Range)
        float MaximumRange = 1000.0f;
    
    private:
    
        float CurrentPercentage;
        float ConsumptionRate;
        float FlashlightRadius;
        float FlashlightIntensity;
        float FlashlightRange;
    
        class ALightsOutCharacter *MyOwner;
        bool IsOn;
        float LerpDirection;
};
