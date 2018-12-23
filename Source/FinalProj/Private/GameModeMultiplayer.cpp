// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModeMultiplayer.h"
#include "PlayerControllerMultiplayer.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"


AGameModeMultiplayer::AGameModeMultiplayer()
{
	loaded = false;
}

void AGameModeMultiplayer::BeginPlay() {
	Super::BeginPlay();
	// 'FCString::Atoi' converts 'FString' to 'int32' and we use the static 'ParseOption' function of the
	// 'UGameplayStatics' Class to get the correct Key from the 'OptionsString'
	UE_LOG(LogTemp, Warning, TEXT("OPTIONS: %s"), *OptionsString);
	expectedPlayerCount = FCString::Atoi(*(UGameplayStatics::ParseOption(OptionsString, "PlayerCount")));
	UE_LOG(LogTemp, Warning, TEXT("PLAYERS: %d"), expectedPlayerCount);
	loaded = true;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), SpawnPoints);

}

bool AGameModeMultiplayer::ReadyToStartMatch_Implementation() {
	if (GetMatchState() == MatchState::WaitingToStart && loaded)
	{
		if (expectedPlayerCount == playercount)
		{
			UE_LOG(LogTemp, Warning, TEXT("READY: %d == %d"), expectedPlayerCount, playercount);
			return true;
		}
	}
	return false;
}

void AGameModeMultiplayer::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	PlayerControllerList.Add(Cast<APlayerControllerMultiplayer>(NewPlayer));
	playercount++;
	GLog->Log("Player Joined");
}

AActor * AGameModeMultiplayer::ChoosePlayerStart_Implementation(AController * Player)
{
	int index = FMath::RoundToInt(FMath::RandRange(0, SpawnPoints.Num()-1));
	AActor* spawn = SpawnPoints[index];
	SpawnPoints.RemoveAt(index);
	return spawn;
}