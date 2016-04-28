// Fill out your copyright notice in the Description page of Project Settings.

#include "LightsOut.h"
#include "Flashlight.h"
#include "Sound/SoundCue.h"
#include "HittableObject.h"
#include "LightsOutCharacter.h"

AFlashlight::AFlashlight()
{
    PrimaryActorTick.bCanEverTick = true;
    
    // Creates a skeletal component for the flashlight and attaches it to this Actor, making it the root component.
    FlashlightMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FlashlightMesh"));
    RootComponent = FlashlightMesh;
    
    // Creates a spotlight component and attaches it to the mesh component.
    SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
    SpotLightComponent->AttachTo(RootComponent);
}

void AFlashlight::BeginPlay()
{
    Super::BeginPlay();
    
    Initialize();
}

void AFlashlight::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // If the flashlight is on the battery life is decreased, the raycast is used to try and intersect
    // with a hittable object, and the light variables are lerpec based on the direction.
    if(IsOn)
    {
        BatteryLife -= DeltaTime * ConsumptionRate;
        CastLight();
        LerpLight(DeltaTime * LerpDirection);
    }
    
    // If the battery is dead and the light is on, this will turn it off.
    if(BatteryLife <= 0 && IsOn)
    {
        ToggleLight();
        UGameplayStatics::SetGamePaused(GetWorld(),true);
    }
}

void AFlashlight::Initialize()
{
    // Initializes the variables based on the initial percentage and turns on the flashlight.
    IsOn = true;
    SpotLightComponent->SetLightColor(LightColor);
    CurrentPercentage = InitialPercentage;
    LerpConsumptionRate(CurrentPercentage);
    LerpRadius(CurrentPercentage);
    LerpIntensity(CurrentPercentage);
    LerpRange(CurrentPercentage);
    
    LerpDirection = 0;
    
    ToggleLight();
}

void AFlashlight::CastLight()
{
    static FName FlashlightCast = FName(TEXT("FlashlightCast"));
    
    // GetWorld()->DebugDrawTraceTag = FlashlightCast;
    
    // Vector math to point the raycast in the right direction.
    FVector StartPosition = GetActorLocation();
    FVector ForwardVector = GetActorForwardVector();
    ForwardVector = ForwardVector.RotateAngleAxis(90, GetActorUpVector());
    ForwardVector.Normalize();
    FVector EndPosition = StartPosition + FlashlightRange * ForwardVector;
    
    // Sets raycast variables to ignore the flashlight and owner Actors.
    FCollisionQueryParams TraceParameters(FlashlightCast, true);
    TraceParameters.AddIgnoredActor(this);
    TraceParameters.AddIgnoredActor(MyOwner);
    TraceParameters.bTraceAsyncScene = true;
    TraceParameters.bReturnPhysicalMaterial = true;
    // TraceParameters.TraceTag = FlashlightCast;
    
    FHitResult Hit(ForceInit);
    GetWorld()->LineTraceSingleByObjectType(Hit, StartPosition, EndPosition,
                                            FCollisionObjectQueryParams::AllObjects, TraceParameters);
    
    // If the thing hit is a HittableObject, then the RespondToFlashlightHit() method is called on
    // that object.
    AHittableObject *HittableObject = Cast<AHittableObject>(Hit.GetActor());
    if(HittableObject != nullptr)
    {
        HittableObject->RespondToFlashlightHit();
    }
}

// Turns the flashlight on/off.
void AFlashlight::ToggleLight()
{
    IsOn = !IsOn;
    SpotLightComponent->SetIntensity(IsOn ? FlashlightIntensity : 0);
    FlashlightAudioComponent = PlaySound(IsOn ? ToggleOnSound : ToggleOffSound);
}

// Updates all of the variables based on a percentage.
void AFlashlight::LerpLight(float DeltaTime)
{
    if(DeltaTime == 0) { return; }
    
    CurrentPercentage += LerpSpeed * DeltaTime;
    CurrentPercentage = FMath::Clamp(CurrentPercentage, 0.0f, 100.0f);
    
    LerpConsumptionRate(CurrentPercentage);
    LerpRadius(CurrentPercentage);
    LerpIntensity(CurrentPercentage);
    LerpRange(CurrentPercentage);
}

void AFlashlight::LerpConsumptionRate(float Percentage)
{
    ConsumptionRate = FMath::Lerp(MinimumConsumptionRate, MaximumConsumptionRate, Percentage / 100.0f);
}

void AFlashlight::LerpRadius(float Percentage)
{
    FlashlightRadius = FMath::Lerp(MinimumRadius, MaximumRadius, 1.0f - Percentage / 100.0f);
    SpotLightComponent->SetInnerConeAngle(FlashlightRadius);
    SpotLightComponent->SetOuterConeAngle(FlashlightRadius * InnerOuterConeRatio);
}

void AFlashlight::LerpIntensity(float Percentage)
{
    FlashlightIntensity = FMath::Lerp(MinimumIntensity, MaximumIntensity, Percentage / 100.0f);
    SpotLightComponent->SetIntensity(FlashlightIntensity);
}

void AFlashlight::LerpRange(float Percentage)
{
    FlashlightRange = FMath::Lerp(MinimumRange, MaximumRange, Percentage / 100.0f);
    SpotLightComponent->SetAttenuationRadius(FlashlightRange);
}

UAudioComponent *AFlashlight::PlaySound(USoundCue *Sound)
{
    UAudioComponent *AC = nullptr;
    if(Sound)
    {
        AC = UGameplayStatics::SpawnSoundAttached(Sound, RootComponent);
    }
    return AC;
}