// Fill out your copyright notice in the Description page of Project Settings.

#include "GameStateMultiplayer.h"
#include "PlayerStateMultiplayer.h"
#include "Net/UnrealNetwork.h"
#include "PlayerStateMultiplayer.h"
#include "../TP_ThirdPerson/TP_ThirdPersonCharacter.h"

AGameStateMultiplayer::AGameStateMultiplayer() 
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGameStateMultiplayer::BeginPlay() {

	KeysCaught = 0;
}

void AGameStateMultiplayer::Tick(float DeltaTime)
{
	if (Role == ROLE_Authority)
	{
		if (MatchState == FName(TEXT("InProgress")))
		{
			UE_LOG(LogTemp, Warning, TEXT("TOTAL PLAYERS: %f"), PlayerArray.Num());
			PlayersLeft = PlayerArray.Num();
			for (int i = 0; i < PlayerArray.Num(); i++)
			{
				APlayerStateMultiplayer* ps = Cast<APlayerStateMultiplayer>(PlayerArray[i]);
				if (ps->HasDied)
				{
					PlayersLeft--;
				}
				if (ps->IsEnemy || ps->HasEscaped)
				{
					PlayersLeft--;
				}
			}
			UE_LOG(LogTemp, Warning, TEXT("PLAYERS LEFT: %f"), PlayersLeft);
		}
	}
}


void AGameStateMultiplayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGameStateMultiplayer, KeysCaught);
}

void AGameStateMultiplayer::KeyCaught()
{
	ServerKeyCaught();
}

void AGameStateMultiplayer::ServerKeyCaught_Implementation()
{
	if (Role == ROLE_Authority)
	{
		KeysCaught++;
	}
}

bool AGameStateMultiplayer::ServerKeyCaught_Validate()
{
	return true;
}

int AGameStateMultiplayer::GetKeysCaught()
{
	return KeysCaught;
}

int AGameStateMultiplayer::GetPlayersLeft()
{
	return PlayersLeft;
}
