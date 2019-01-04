// Fill out your copyright notice in the Description page of Project Settings.

#include "GameStateMultiplayer.h"
#include "Net/UnrealNetwork.h"

void AGameStateMultiplayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiplayer1Character, KeysCaught);
}