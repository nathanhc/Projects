// Copyright 1998-2015 Epic Games, Inc. All Rights Reserved.

#include "LightsOut.h"
#include "Flashlight.h"
#include "LightsOutCharacter.h"
#include "LightsOutProjectile.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/InputSettings.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ALightsOutCharacter

ALightsOutCharacter::ALightsOutCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->AttachParent = GetCapsuleComponent();
	FirstPersonCameraComponent->RelativeLocation = FVector(0, 0, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->AttachParent = FirstPersonCameraComponent;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
    
	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->AttachTo(Mesh1P, TEXT("GripPoint"), EAttachLocation::SnapToTargetIncludingScale, true);


	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)
    
    //initializing the speed of the character
    mDefaultSpeed = this->GetCharacterMovement()->MaxWalkSpeed;
    mMaxSpeed = mDefaultSpeed * mMaxSpeedMulti;
    mCurrentSpeed = mDefaultSpeed;
}

void ALightsOutCharacter::BeginPlay()
{
    Super::BeginPlay();
    
    if(FlashlightClass)
    {
        UWorld *World = GetWorld();
        if(World)
        {
            FActorSpawnParameters SpawnParameters;
            SpawnParameters.Owner = this;
            SpawnParameters.Instigator = Instigator;
            
            FRotator Rotation(0.0f, 0.0f, 0.0f);
            Flashlight = World->SpawnActor<AFlashlight>(FlashlightClass, FVector(0, 0, 0), Rotation, SpawnParameters);
            if(Flashlight)
            {
                Flashlight->GetFlashlightMesh()->AttachTo(Mesh1P, TEXT("GripPoint"), EAttachLocation::SnapToTargetIncludingScale, true);
                Flashlight->SetMyOwner(this);
            }
        }
    }
}

void ALightsOutCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ALightsOutCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	check(InputComponent);

	InputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	InputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	InputComponent->BindAxis("MoveForward", this, &ALightsOutCharacter::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ALightsOutCharacter::MoveRight);
	
	InputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	InputComponent->BindAxis("TurnRate", this, &ALightsOutCharacter::TurnAtRate);
	InputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	InputComponent->BindAxis("LookUpRate", this, &ALightsOutCharacter::LookUpAtRate);
    
    InputComponent->BindAction("Run", IE_Pressed, this, &ALightsOutCharacter::OnRun);
    InputComponent->BindAction("Run", IE_Released, this, &ALightsOutCharacter::OnWalk);
    
    InputComponent->BindAction("FlashlightToggle", IE_Pressed, this, &ALightsOutCharacter::OnToggleFlashlight);
    InputComponent->BindAxis("FocusFlashlight", this, &ALightsOutCharacter::OnFocusFlashlight);
}
void ALightsOutCharacter::OnWalk()
{
    GetWorldTimerManager().ClearTimer(RunTimer);
    GetWorldTimerManager().SetTimer(WalkTimer, this, &ALightsOutCharacter::PlayerWalk, TimerRate, true);
}
void ALightsOutCharacter::OnRun()
{
    GetWorldTimerManager().ClearTimer(WalkTimer);
    GetWorldTimerManager().SetTimer(RunTimer, this, &ALightsOutCharacter::PlayerRun, TimerRate, true);
}
void ALightsOutCharacter::PlayerRun()
{
    if(GetInputAxisValue("MoveForward") != 0 || GetInputAxisValue("MoveRight") != 0)
    {
        mCurrentSpeed += mCurrentSpeed/10.0f;
        mCurrentSpeed = FMath::Clamp(mCurrentSpeed, mDefaultSpeed, mMaxSpeed);
    }
    else
    {
        mCurrentSpeed = mDefaultSpeed;
    }
    this->GetCharacterMovement()->MaxWalkSpeed = mCurrentSpeed;
}
void ALightsOutCharacter::PlayerWalk()
{
    mCurrentSpeed -= mCurrentSpeed/20.0f;
    mCurrentSpeed = FMath::Clamp(mCurrentSpeed, mDefaultSpeed, mMaxSpeed);
    this->GetCharacterMovement()->MaxWalkSpeed = mCurrentSpeed;
    if(mCurrentSpeed == mDefaultSpeed)
    {
        GetWorldTimerManager().ClearTimer(WalkTimer);
    }
}
void ALightsOutCharacter::OnFire()
{ 
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		const FRotator SpawnRotation = GetControlRotation();
		// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
		const FVector SpawnLocation = GetActorLocation() + SpawnRotation.RotateVector(GunOffset);

		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			// spawn the projectile at the muzzle
			World->SpawnActor<ALightsOutProjectile>(ProjectileClass, SpawnLocation, SpawnRotation);
		}
	}

	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if(FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		if(AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void ALightsOutCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if( TouchItem.bIsPressed == true )
	{
		return;
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ALightsOutCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	if( ( FingerIndex == TouchItem.FingerIndex ) && (TouchItem.bMoved == false) )
	{
		OnFire();
	}
	TouchItem.bIsPressed = false;
}

void ALightsOutCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if ((TouchItem.bIsPressed == true) && ( TouchItem.FingerIndex==FingerIndex))
	{
		if (TouchItem.bIsPressed)
		{
			if (GetWorld() != nullptr)
			{
				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
				if (ViewportClient != nullptr)
				{
					FVector MoveDelta = Location - TouchItem.Location;
					FVector2D ScreenSize;
					ViewportClient->GetViewportSize(ScreenSize);
					FVector2D ScaledDelta = FVector2D( MoveDelta.X, MoveDelta.Y) / ScreenSize;									
					if (ScaledDelta.X != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.X * BaseTurnRate;
						AddControllerYawInput(Value);
					}
					if (ScaledDelta.Y != 0.0f)
					{
						TouchItem.bMoved = true;
						float Value = ScaledDelta.Y* BaseTurnRate;
						AddControllerPitchInput(Value);
					}
					TouchItem.Location = Location;
				}
				TouchItem.Location = Location;
			}
		}
	}
}

void ALightsOutCharacter::OnToggleFlashlight()
{
    if(Flashlight)
    {
        Flashlight->ToggleLight();
    }
}

void ALightsOutCharacter::OnFocusFlashlight(float axis)
{
    if(Flashlight)
    {
        Flashlight->SetLerp(axis);
    }
}

void ALightsOutCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ALightsOutCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ALightsOutCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ALightsOutCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ALightsOutCharacter::EnableTouchscreenMovement(class UInputComponent* InputComponent)
{
	bool bResult = false;
	if(FPlatformMisc::GetUseVirtualJoysticks() || GetDefault<UInputSettings>()->bUseMouseForTouch )
	{
		bResult = true;
		InputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ALightsOutCharacter::BeginTouch);
		InputComponent->BindTouch(EInputEvent::IE_Released, this, &ALightsOutCharacter::EndTouch);
		InputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ALightsOutCharacter::TouchUpdate);
	}
	return bResult;
}
