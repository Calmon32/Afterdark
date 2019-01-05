// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerStateMultiplayer.h"
#include "Net/UnrealNetwork.h"

APlayerStateMultiplayer::APlayerStateMultiplayer()
{
	IsEnemy = false;
}

void APlayerStateMultiplayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerStateMultiplayer, IsEnemy);
	DOREPLIFETIME(APlayerStateMultiplayer, HasEscaped);
	DOREPLIFETIME(APlayerStateMultiplayer, HasDied);
}