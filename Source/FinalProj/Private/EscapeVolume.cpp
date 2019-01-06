// Fill out your copyright notice in the Description page of Project Settings.

#include "EscapeVolume.h"
#include "Components/BoxComponent.h"
#include "PlayerStateMultiplayer.h"
#include "../TP_ThirdPerson/TP_ThirdPersonCharacter.h"

// Sets default values
AEscapeVolume::AEscapeVolume()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BoxCollider = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollider"));
	BoxCollider->SetupAttachment(RootComponent);

	BoxCollider->OnComponentBeginOverlap.AddDynamic(this, &AEscapeVolume::OnOverlap);

}

void AEscapeVolume::OnOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	if (Role == ROLE_Authority)
	{
		ATP_ThirdPersonCharacter* const player = Cast<ATP_ThirdPersonCharacter>(OtherActor);

		if (OtherActor != this && player != NULL)
		{
			GLog->Log("PLAYER!");
			APlayerStateMultiplayer* ps = Cast<APlayerStateMultiplayer>(player->GetPlayerState());
			if (ps != NULL)
			{
				ps->HasEscaped = true;
				UE_LOG(LogTemp, Warning, TEXT("HAS ESCAPED!!"));
			}
		}
	}
}

// Called when the game starts or when spawned
void AEscapeVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEscapeVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

