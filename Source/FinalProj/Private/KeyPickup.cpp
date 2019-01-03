// Fill out your copyright notice in the Description page of Project Settings.

#include "KeyPickup.h"
#include "Components/StaticMeshComponent.h"
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
		SetLifeSpan(1.0f);
	}
}

