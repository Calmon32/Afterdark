// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModeMultiplayer.h"
#include "PlayerControllerMultiplayer.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Engine/World.h"


AGameModeMultiplayer::AGameModeMultiplayer()
{
	loaded = false;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), SpawnPoints);
}

void AGameModeMultiplayer::BeginPlay() {
	Super::BeginPlay();
	// 'FCString::Atoi' converts 'FString' to 'int32' and we use the static 'ParseOption' function of the
	// 'UGameplayStatics' Class to get the correct Key from the 'OptionsString'
	UE_LOG(LogTemp, Warning, TEXT("OPTIONS: %s"), *OptionsString);
	expectedPlayerCount = FCString::Atoi(*(UGameplayStatics::ParseOption(OptionsString, "PlayerCount")));
	UE_LOG(LogTemp, Warning, TEXT("PLAYERS: %d"), expectedPlayerCount);
	isServer = FCString::Atoi(*(UGameplayStatics::ParseOption(OptionsString, "IsServer")));
	UE_LOG(LogTemp, Warning, TEXT("IsServer: %d"), isServer);
	enemyPlayer = FCString::Atoi(*(UGameplayStatics::ParseOption(OptionsString, "EnemyPlayer")));
	UE_LOG(LogTemp, Warning, TEXT("EnemyPlayer: %d"), enemyPlayer);

	UE_LOG(LogTemp, Warning, TEXT("MAPNAME: %s"), *GetWorld()->GetMapName());

	loaded = true;

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
	if (playercount == enemyPlayer)
	{
		if (IsValid(NewPlayer->GetPawn())) {
			Destroy(NewPlayer->GetPawn());
			FVector Location(0.0f, 0.0f, 0.0f);
			FRotator Rotation(0.0f, 0.0f, 0.0f);
			FActorSpawnParameters SpawnInfo;
			AActor* const SpawnedPickup = GetWorld()->SpawnActor<AActor>(EnemyPawn, Location, Rotation, SpawnInfo);
		}
	}
	GLog->Log("Player Joined");
}

FString AGameModeMultiplayer::InitNewPlayer(APlayerController * NewPlayerController, const FUniqueNetIdRepl & UniqueId, const FString & Options, const FString & Portal)
{
	Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	UE_LOG(LogTemp, Warning, TEXT("PlayerOptions: %s"), *Options);

	FString playername = UGameplayStatics::ParseOption(Options, TEXT("Name"));
	ChangeName(NewPlayerController, playername, true);

	/*
	bool IsEnemy = FCString::Stricmp(*UGameplayStatics::ParseOption(Options, TEXT("IsEnemy")), TEXT("1")) == 0;
	if (IsEnemy) UE_LOG(LogTemp, Warning, TEXT("IsEnemy!"));
	*/
	return FString();
}

AActor * AGameModeMultiplayer::ChoosePlayerStart_Implementation(AController * Player)
{
	int index = FMath::RoundToInt(FMath::RandRange(0, SpawnPoints.Num()-1));
	AActor* spawn = SpawnPoints[index];
	SpawnPoints.RemoveAt(index);
	return spawn;	
}
