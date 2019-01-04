// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerControllerMultiplayer.h"
#include "Kismet/GameplayStatics.h"

void APlayerControllerMultiplayer::ConnectToHostLobby(FString ipAddress)
{
	UWorld* world = this->GetWorld();
	if (world)
	{
		//This should work in theory...?
		APlayerController* pc = UGameplayStatics::GetPlayerController(this, 0);

		//UGameplayStatics::OpenLevel(world, FName(TEXT("Lobby")));

		UE_LOG(LogTemp, Warning, TEXT("ConnectToHostLobby >> Connecting to host."));
		pc->ClientTravel(ipAddress, TRAVEL_Absolute);
	}
}