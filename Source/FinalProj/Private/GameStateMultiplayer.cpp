// Fill out your copyright notice in the Description page of Project Settings.

#include "GameStateMultiplayer.h"
#include "PlayerStateMultiplayer.h"
#include "Net/UnrealNetwork.h"
#include "PlayerStateMultiplayer.h"
#include "../TP_ThirdPerson/TP_ThirdPersonCharacter.h"

AGameStateMultiplayer::AGameStateMultiplayer() 
{
	PrimaryActorTick.bCanEverTick = true;
	IsMatchOver = false;
}

void AGameStateMultiplayer::BeginPlay() {

	KeysCaught = 0;
	NotSurvivors = 0;
}

void AGameStateMultiplayer::Tick(float DeltaTime)
{
	if (Role == ROLE_Authority)
	{
		if (MatchState == FName(TEXT("InProgress")))
		{
			
		}
	}
}


void AGameStateMultiplayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGameStateMultiplayer, KeysCaught);
	DOREPLIFETIME(AGameStateMultiplayer, IsMatchOver);
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
