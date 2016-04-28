// Fill out your copyright notice in the Description page of Project Settings.

#include "LightsOut.h"
#include "PushLightGem.h"


// Sets default values
APushLightGem::APushLightGem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GemMesh"));
	RootComponent = GemMesh;

	PointLightComponent = CreateDefaultSubobject<UPointLightComponent>(TEXT("PointLight"));
	PointLightComponent->AttachTo(RootComponent);

	LightColor = FColor(255, 255, 255, 255);

}

// Called when the game starts or when spawned
void APushLightGem::BeginPlay()
{
	Super::BeginPlay();

	m_IsShining = true;
	PointLightComponent->SetIntensity(LightIntensity);
	PointLightComponent->SetLightColor(LightColor, true);
}

// Called every frame
void APushLightGem::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

