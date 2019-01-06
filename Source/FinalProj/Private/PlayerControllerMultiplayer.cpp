// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerControllerMultiplayer.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerStateMultiplayer.h"

APlayerControllerMultiplayer::APlayerControllerMultiplayer() 
{
	IsInputDisabled = false;
}

void APlayerControllerMultiplayer::Tick(float DeltaTime) 
{
	if (IsInputDisabled)
	{
		PlayerInput = nullptr;
	}
}

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

void APlayerControllerMultiplayer::DisableMove_Implementation()
{
	GLog->Log("DISABLED!");
	IsInputDisabled = true;
}

bool APlayerControllerMultiplayer::DisableMove_Validate()
{
	return true;
}

void APlayerControllerMultiplayer::ChangeState_Spectator()
{
	ChangeState(NAME_Spectating);
	if (Role == ROLE_Authority && PlayerState != NULL)
	{
		Cast<APlayerStateMultiplayer>(PlayerState)->bIsSpectator = true;
	}
}

void APlayerControllerMultiplayer::ChangeState_Player()
{
	ChangeState(NAME_Playing);
	if (Role == ROLE_Authority && PlayerState != NULL)
	{
		Cast<APlayerStateMultiplayer>(PlayerState)->bIsSpectator = false;
	}
}

void APlayerControllerMultiplayer::EnableMove_Implementation()
{
	GLog->Log("ENABLED!");
	IsInputDisabled = false;
}

bool APlayerControllerMultiplayer::EnableMove_Validate()
{
	return true;
}
