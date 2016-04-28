// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "PushLightGem.generated.h"

UCLASS()
class LIGHTSOUT_API APushLightGem : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APushLightGem();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

protected:
	UPROPERTY(VisibleDefaultsOnly, Category = Components)
		UStaticMeshComponent *GemMesh;

	UPROPERTY(EditDefaultsOnly, Category = Sound)
	class USoundCue* pitch;

	UPROPERTY(EditDefaultsOnly, Category = Light)
		UPointLightComponent *PointLightComponent;

	UPROPERTY(EditDefaultsOnly, Category = Light)
		FColor LightColor;

	UPROPERTY(EditDefaultsOnly, Category = Light)
		float LightIntensity = 5000.0f;

private:
	bool m_IsShining;
	
};
