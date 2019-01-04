// Fill out your copyright notice in the Description page of Project Settings.

#include "KeyPickup.h"
#include "Components/StaticMeshComponent.h"
#include "Public/GameStateMultiplayer.h"
#include "Net/UnrealNetwork.h"

AKeyPickup::AKeyPickup() {
	bReplicateMovement = true;
	GetStaticMeshComponent()->SetMobility(EComponentMobility::Stationary);
	GetStaticMeshComponent()->SetSimulatePhysics(true);

}

void AKeyPickup::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AKeyPickup, BatteryPower);
}

void AKeyPickup::PickedUpBy(APawn * Pawn)
{
	Super::PickedUpBy(Pawn);
	if (Role == ROLE_Authority) {
		AGameStateMultiplayer* const gamestate = GetWorld() != NULL ? GetWorld()->GetGameState<AGameStateMultiplayer>() : NULL;
		GLog->Log("add to keys");
		gamestate->KeyCaught();
		Destroy(this);
	}
}

