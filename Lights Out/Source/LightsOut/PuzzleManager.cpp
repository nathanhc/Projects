// Fill out your copyright notice in the Description page of Project Settings.

#include "LightsOut.h"
#include "PuzzleManager.h"


// Sets default values
APuzzleManager::APuzzleManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APuzzleManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void APuzzleManager::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );
}

void APuzzleManager::SpawnPuzzle()
{

}

void APuzzleManager::OnCompletePuzzle()
{

}

void APuzzleManager::OnFailPuzzle()
{

}

bool APuzzleManager::CheckIsSolved()
{
	return false;
}

