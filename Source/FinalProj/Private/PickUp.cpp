// Fill out your copyright notice in the Description page of Project Settings.

#include "PickUp.h"
#include "Net/UnrealNetwork.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
APickUp::APickUp()
{
	bReplicates = true;

	PrimaryActorTick.bCanEverTick = false;

	GetStaticMeshComponent()->SetGenerateOverlapEvents(true);

	if (Role == ROLE_Authority) {
		bIsActive = true;
	}

}

// Called when the game starts or when spawned
void APickUp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const {
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APickUp, bIsActive);
}

bool APickUp::IsActive() {
	return bIsActive;
}

void APickUp::SetActive(bool NewPickupState) {
	if (Role == ROLE_Authority) {
		bIsActive = NewPickupState;
	}
}

void APickUp::WasCollected_Implementation()
{
	UE_LOG(LogClass, Log, TEXT("APickup::WasCollected_Implementation %s"), *GetName());
}

void APickUp::PickedUpBy(APawn* Pawn) {
	if (Role == ROLE_Authority) {
		PickupInstigator = Pawn;
		PendingKill = true;
		ClientOnPickedUpBy(Pawn);
	}
}

bool APickUp::IsPendingKill()
{
	return PendingKill;
}

void APickUp::ClientOnPickedUpBy_Implementation(APawn* Pawn) {
	PickupInstigator = Pawn;
	WasCollected();
}

void APickUp::OnRep_IsActive() {

}