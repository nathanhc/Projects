// Fill out your copyright notice in the Description page of Project Settings.

#include "LightsOut.h"
#include "HittableObject.h"

AHittableObject::AHittableObject()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AHittableObject::BeginPlay()
{
	Super::BeginPlay();
}

void AHittableObject::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void AHittableObject::RespondToFlashlightHit()
{
    
}

