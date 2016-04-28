// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PuzzleManager.generated.h"

UCLASS()
class LIGHTSOUT_API APuzzleManager : public AActor
{
	GENERATED_BODY()
	
    public:	
        // Sets default values for this actor's properties
        APuzzleManager();

        // Called when the game starts or when spawned
        virtual void BeginPlay() override;
        
        // Called every frame
        virtual void Tick( float DeltaSeconds ) override;

        virtual void SpawnPuzzle();
        virtual void OnCompletePuzzle();
        virtual void OnFailPuzzle();
        virtual bool CheckIsSolved();
	
};
