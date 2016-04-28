// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "PuzzleManager.h"
#include "FirstRoom.generated.h"

/**
 * The FirstRoom class inherits from the PuzzleManager class to have all of the necessary functions for solving a puzzle.
 */
UCLASS()
class LIGHTSOUT_API AFirstRoom : public APuzzleManager
{
	GENERATED_BODY()
	
    public:
        virtual void BeginPlay() override;
        virtual void Tick(float DeltaTime) override;
        virtual void SpawnPuzzle() override;
        virtual void OnCompletePuzzle() override;
        virtual void OnFailPuzzle() override;
        virtual bool CheckIsSolved() override;
		class UAudioComponent *PlaySound(class USoundCue *Sound);

    public:
        bool CheckSequence(class ASoundGem *LitGem);
    
    protected:
		UPROPERTY(Transient)
		class UAudioComponent *SolvedAudioComponent;

		UPROPERTY(EditAnywhere, Category = Sound)
		class USoundCue *DoorSound;
    
        UPROPERTY(EditAnywhere, Category = Sound)
        class USoundCue *FailSound;

        UPROPERTY(EditAnywhere)
        TArray<class ASoundGem*> SoundGems;
    
        UPROPERTY(EditAnywhere)
        class AActor *Door;
    
        UPROPERTY(EditAnywhere)
        class ALightsOutCharacter* Character;
    
    private:
        int CurrentGoal;
        bool IsSolved;
        bool HasFailed;
};
