// Fill out your copyright notice in the Description page of Project Settings.

#include "Trap.h"
#include "Components/SphereComponent.h"
#include "PlayerControllerMultiplayer.h"
#include "TimerManager.h"
#include "../TP_ThirdPerson/TP_ThirdPersonCharacter.h"

// Sets default values
ATrap::ATrap()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollider = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollider"));
	SphereCollider->SetupAttachment(RootComponent);
	SphereCollider->SetSphereRadius(30.0f);

	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &ATrap::OnOverlap);

}

void ATrap::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Role == ROLE_Authority)
	{
		ATP_ThirdPersonCharacter* const player = Cast<ATP_ThirdPersonCharacter>(OtherActor);

		if (OtherActor != this && player != NULL)
		{
			GLog->Log("TRAPPED!");
			APlayerControllerMultiplayer* controller = Cast<APlayerControllerMultiplayer>(player->GetController());
			if (controller != NULL)
			{
				TrappedPlayer = controller;
				controller->DisableMove();
				FTimerHandle UnusedHandle;
				GetWorldTimerManager().SetTimer(UnusedHandle, this, &ATrap::DestroyTrap, 4.0f, false);
			}
		}
	}
}

// Called when the game starts or when spawned
void ATrap::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATrap::DestroyTrap()
{
	if (TrappedPlayer != NULL)
	{
		TrappedPlayer->EnableMove();
		Destroy(this);
	}
}

// Called every frame
void ATrap::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

