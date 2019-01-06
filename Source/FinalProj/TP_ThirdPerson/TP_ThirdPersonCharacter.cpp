// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TP_ThirdPersonCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Public/GameStateMultiplayer.h"
#include "PlayerStateMultiplayer.h"
#include "Net/UnrealNetwork.h"
#include "Pickup.h"
#include "Vector.h"
#include "KeyPickup.h"
#include "Door.h"
//#include "Runtime/Engine/Classes/Engine/World.h"


//////////////////////////////////////////////////////////////////////////
// ATP_ThirdPersonCharacter

ATP_ThirdPersonCharacter::ATP_ThirdPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	CollectionSphereRadius = 200.f;

	CollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollectionSphere"));
	CollectionSphere->SetupAttachment(RootComponent);
	CollectionSphere->SetSphereRadius(CollectionSphereRadius);

	isCrouched = false;

	IsInteractButtonPressed = false;
	HoldInteractTime = 0.0f;

	TotalHealth = 2;
	CurrentHealth = TotalHealth;

	UCharacterMovementComponent* Charmove = GetCharacterMovement();
	Charmove->MaxWalkSpeed = WalkSpeed;

}

void ATP_ThirdPersonCharacter::Tick(float DeltaTime)
{
	if (Role == ROLE_Authority)
	{
		AGameStateMultiplayer* const gamestate = GetWorld() != NULL ? GetWorld()->GetGameState<AGameStateMultiplayer>() : NULL;
		if (gamestate->GetKeysCaught() >= 3 && IsInteractButtonPressed && IsValid(DoorInteract))
		{
			HoldInteractTime += DeltaTime;
			UE_LOG(LogTemp, Warning, TEXT("TIMER: %f"), HoldInteractTime);
			if (HoldInteractTime >= 8.0f)
			{
				UE_LOG(LogTemp, Warning, TEXT("OPENNED DOOR"));
				DoorInteract->OpenDoor();
				IsInteractButtonPressed = false;
			}
		}
		else
		{
			HoldInteractTime = 0.0f;
		}
	}
}

void ATP_ThirdPersonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATP_ThirdPersonCharacter, CollectionSphereRadius);
	DOREPLIFETIME(ATP_ThirdPersonCharacter, HoldInteractTime);
	DOREPLIFETIME(ATP_ThirdPersonCharacter, TotalHealth);
	DOREPLIFETIME(ATP_ThirdPersonCharacter, CurrentHealth);

}

void ATP_ThirdPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &ATP_ThirdPersonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATP_ThirdPersonCharacter::MoveRight);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ATP_ThirdPersonCharacter::ToggleCrouch);
	PlayerInputComponent->BindAction("CollectPickups", IE_Pressed, this, &ATP_ThirdPersonCharacter::CollectPickups);
	PlayerInputComponent->BindAction("CollectPickups", IE_Released, this, &ATP_ThirdPersonCharacter::ReleasedButton);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ATP_ThirdPersonCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ATP_ThirdPersonCharacter::LookUpAtRate);

}


void ATP_ThirdPersonCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ATP_ThirdPersonCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void ATP_ThirdPersonCharacter::SetEscaped()
{
	if (Role == ROLE_Authority)
	{
		Cast<APlayerStateMultiplayer>(GetPlayerState())->HasEscaped = true;
		Destroy(this);
	}
}

void ATP_ThirdPersonCharacter::MoveForward(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void ATP_ThirdPersonCharacter::MoveRight(float Value)
{
	if ( (Controller != NULL) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void ATP_ThirdPersonCharacter::ToggleCrouch()
{
	ServerToggleCrouch();
	UCharacterMovementComponent* Charmove = GetCharacterMovement();
	if (isCrouched) {
		Charmove->MaxWalkSpeed = CrouchSpeed;
		GLog->Log("is Crouch");
	}
	else {
		Charmove->MaxWalkSpeed = WalkSpeed;
		GLog->Log("is NOT Crouch");
	}
	//GLog->Log("iCrouch");
		
		//CrouchButtonDown = true;
}

void ATP_ThirdPersonCharacter::DealDamage()
{
	if (Role == ROLE_Authority)
	{
		CurrentHealth--;
	}
}

void ATP_ThirdPersonCharacter::CollectPickups() {
	ServerCollectPickups();
}

bool ATP_ThirdPersonCharacter::ServerCollectPickups_Validate() {
	return true;
}

void ATP_ThirdPersonCharacter::ServerToggleCrouch_Implementation()
{
	if (Role == ROLE_Authority) {
		//GLog->Log("is server toggle crouch");
		UCharacterMovementComponent* Charmove = GetCharacterMovement();
		if (isCrouched) {
			Charmove->MaxWalkSpeed = CrouchSpeed;
			//GLog->Log("is Crouch");
			isCrouched = false;
		}
		else {
			Charmove->MaxWalkSpeed = WalkSpeed;
			//GLog->Log("is NOT Crouch");
			isCrouched = true;
		}
		
	}
}

bool ATP_ThirdPersonCharacter::ServerToggleCrouch_Validate()
{
	return true;
}

void ATP_ThirdPersonCharacter::DoThisShit_Implementation(APickUp * pickup)
{
	CollectedPickup(pickup);
}

void ATP_ThirdPersonCharacter::ReleasedButton()
{
	ServerReleasedButton();
}

void ATP_ThirdPersonCharacter::ServerReleasedButton_Implementation()
{
	if (Role == ROLE_Authority)
	{
		IsInteractButtonPressed = false;
	}
}

bool ATP_ThirdPersonCharacter::ServerReleasedButton_Validate()
{
	return true;
}

void ATP_ThirdPersonCharacter::ServerCollectPickups_Implementation()
{
	if (Role == ROLE_Authority)
	{
		IsInteractButtonPressed = true;

		TArray<AActor*> CollectedActors;
		CollectionSphere->GetOverlappingActors(CollectedActors);

		for (int i = 0; i < CollectedActors.Num(); i++)
		{
			APickUp* const TestPickup = Cast<APickUp>(CollectedActors[i]);
			if (TestPickup != NULL && !TestPickup->IsPendingKill() && TestPickup->IsActive())
			{
				if (AKeyPickup* const TestBattery = Cast<AKeyPickup>(TestPickup))
				{

					//ADoor* door->AddProgress();

					/*UE_LOG(LogTemp, Warning, TEXT("number of quests: %d"), QuestArray.Num());
					for (int j = 0; j < QuestArray.Num(); j++)
					{
						AQuestBase* quest = QuestArray[j];
						UE_LOG(LogTemp, Warning, TEXT("description of quests: %d"), quest->TargetID);
						if (quest->TargetID == TestBattery->PickupID)
						{
							quest->AddProgress();
							GLog->Log("add to progress asdhjasgdkjahsdfasd");
						}
					}
					LevelingSystem->AddExperience(100.0f);
					*/
				}
				CollectedPickup(TestPickup);
				TestPickup->PickedUpBy(this);
				TestPickup->SetActive(false);
			}
		}
	}
}