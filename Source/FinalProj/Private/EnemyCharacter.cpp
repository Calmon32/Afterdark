#include "EnemyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SphereComponent.h"
#include "Trap.h"
#include "DrawDebugHelpers.h"
#include "Net/UnrealNetwork.h"

AEnemyCharacter::AEnemyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
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

	IsInteractButtonPressed = false;

}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (Role == ROLE_Authority)
	{
		if (IsInteractButtonPressed)
		{
			HoldInteractTime += DeltaTime;
			UE_LOG(LogTemp, Warning, TEXT("TIMER: %f"), HoldInteractTime);
			if (HoldInteractTime >= 4.0f)
			{
				UE_LOG(LogTemp, Warning, TEXT("Trap Placed!"));
				PlaceTrap();
				IsInteractButtonPressed = false;
			}
		}
		else
		{
			HoldInteractTime = 0.0f;
		}
	}
}

void AEnemyCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEnemyCharacter, AvailableTraps);
	DOREPLIFETIME(AEnemyCharacter, HoldInteractTime);
}

void AEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AEnemyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AEnemyCharacter::MoveRight);

	PlayerInputComponent->BindAction("CollectPickups", IE_Pressed, this, &AEnemyCharacter::PressedButton);
	PlayerInputComponent->BindAction("CollectPickups", IE_Released, this, &AEnemyCharacter::ReleasedButton);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}

void AEnemyCharacter::BasicAttack()
{
	ServerBasicAttack();
}

void AEnemyCharacter::ServerBasicAttack_Implementation()
{
	if (Role == ROLE_Authority)
	{
		const FName TraceTag("MyTraceTag");

		GetWorld()->DebugDrawTraceTag = TraceTag;

		FCollisionQueryParams CollisionParams;
		CollisionParams.TraceTag = TraceTag;

		FCollisionShape Shape = FCollisionShape::MakeBox(FVector(50, 100, 50));
		TArray<FHitResult> SweepResults;
		FVector EndLocation = GetActorLocation() + (GetActorRotation().Vector() * 100);
		GetWorld()->SweepMultiByChannel(SweepResults, GetActorLocation(), EndLocation, GetActorRotation().Quaternion(), ECC_Pawn, Shape, CollisionParams);
		for (int i = 0; i < SweepResults.Num(); i++)
		{
			UE_LOG(LogTemp, Warning, TEXT("COLLISION: %s"), *SweepResults[i].GetActor()->GetName());
		}
	}
}

bool AEnemyCharacter::ServerBasicAttack_Validate()
{
	return true;
}

void AEnemyCharacter::PlaceTrap()
{
	if (Role == ROLE_Authority)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = Instigator;

		FVector SpawnLocation = GetActorLocation();
		FVector Pos = SpawnLocation;

		FHitResult OutHit;
		FVector DownVector = FVector(0, 0, -1);
		FVector End = ((DownVector * 10000000.f) + SpawnLocation);

		FCollisionQueryParams CollisionParams;

		FRotator SpawnRotation;
		SpawnRotation = GetActorRotation();

		DrawDebugLine(GetWorld(), SpawnLocation, End, FColor::Green, false, 1, 0, 1);

		if (GetWorld()->LineTraceSingleByChannel(OutHit, SpawnLocation, End, ECC_Visibility, CollisionParams))
		{
			if (OutHit.bBlockingHit)
			{
				Pos = OutHit.ImpactPoint;
				//OutHit.ImpactNormal
				//SpawnRotation.Vector
			}
		}

		ATrap* const SpawnedPickup = GetWorld()->SpawnActor<ATrap>(WhatToSpawn, Pos, SpawnRotation, SpawnParams);

		GLog->Log("Added One");
	}
}


void AEnemyCharacter::MoveForward(float Value)
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

void AEnemyCharacter::MoveRight(float Value)
{
	if ((Controller != NULL) && (Value != 0.0f))
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

void AEnemyCharacter::ReleasedButton()
{
	ServerReleasedButton();
}

void AEnemyCharacter::ServerReleasedButton_Implementation()
{
	if (Role == ROLE_Authority)
	{
		IsInteractButtonPressed = false;
	}
}

bool AEnemyCharacter::ServerReleasedButton_Validate()
{
	return true;
}

void AEnemyCharacter::PressedButton()
{
	ServerPressedButton();
}

void AEnemyCharacter::ServerPressedButton_Implementation()
{
	if (Role == ROLE_Authority)
	{
		IsInteractButtonPressed = true;
	}
}

bool AEnemyCharacter::ServerPressedButton_Validate()
{
	return true;
}
