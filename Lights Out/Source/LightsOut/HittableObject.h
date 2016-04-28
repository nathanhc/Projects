// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "HittableObject.generated.h"

UCLASS()
class LIGHTSOUT_API AHittableObject : public AActor
{
	GENERATED_BODY()
	
    public:
        AHittableObject();
        virtual void BeginPlay() override;
        virtual void Tick( float DeltaSeconds ) override;

    public:
        virtual void RespondToFlashlightHit();
};
