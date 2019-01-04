// Fill out your copyright notice in the Description page of Project Settings.

#include "GameStateMultiplayer.h"
#include "Net/UnrealNetwork.h"

void AGameStateMultiplayer::BeginPlay() {
	KeysCaught = 0;
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
