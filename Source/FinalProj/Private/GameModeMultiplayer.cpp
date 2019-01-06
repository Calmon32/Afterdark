// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModeMultiplayer.h"
#include "PlayerControllerMultiplayer.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerStateMultiplayer.h"
#include "PlayerControllerMultiplayer.h"
#include "GameStateMultiplayer.h"
#include "Runtime/Engine/Classes/Engine/World.h"


AGameModeMultiplayer::AGameModeMultiplayer()
{
	PrimaryActorTick.bCanEverTick = true;

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
	enemyPlayer = UGameplayStatics::ParseOption(OptionsString, "EnemyPlayer");
	UE_LOG(LogTemp, Warning, TEXT("EnemyPlayer: %s"), *enemyPlayer);

	UE_LOG(LogTemp, Warning, TEXT("MAPNAME: %s"), *GetWorld()->GetMapName());

	AGameStateMultiplayer* gamesta = Cast<AGameStateMultiplayer>(GameState);
	gamesta->ExpectedPlayerCount = expectedPlayerCount;

	loaded = true;

}

void AGameModeMultiplayer::Tick(float DeltaTime)
{

}



void AGameModeMultiplayer::CloseGame()
{
	if (Role == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("MATCH IS OVER!!!!!!!!"));
		//FGenericPlatformMisc::RequestExit(true);
	}
}

bool AGameModeMultiplayer::ReadyToStartMatch_Implementation() {
	if (GetMatchState() == MatchState::WaitingToStart && loaded)
	{
		UE_LOG(LogTemp, Warning, TEXT("READY: %d == %d"), expectedPlayerCount, playercount);
		if (expectedPlayerCount == playercount)
		{
			UE_LOG(LogTemp, Warning, TEXT("MATCH STARTED!!!!!!!!"));
			return true;
		}
	}
	return false;
}

void AGameModeMultiplayer::PostLogin(APlayerController * NewPlayer)
{
	Super::PostLogin(NewPlayer);
	APlayerControllerMultiplayer* pcontroller = Cast<APlayerControllerMultiplayer>(NewPlayer);
	PlayerControllerList.Add(pcontroller);
	APlayerStateMultiplayer* playerstate = Cast<APlayerStateMultiplayer>(NewPlayer->PlayerState);
	playerstate->IsEnemy = pcontroller->IsEnemy;
	
	GLog->Log("Player Joined");
}

FString AGameModeMultiplayer::InitNewPlayer(APlayerController * NewPlayerController, const FUniqueNetIdRepl & UniqueId, const FString & Options, const FString & Portal)
{
	Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
	UE_LOG(LogTemp, Warning, TEXT("PlayerOptions: %s"), *Options);

	FString playername = UGameplayStatics::ParseOption(Options, TEXT("PlayerName"));
	APlayerControllerMultiplayer* pcontroller = Cast<APlayerControllerMultiplayer>(NewPlayerController);

	if(DebugMode) playername = "Lucas";

	ChangeName(NewPlayerController, playername, false);

	UE_LOG(LogTemp, Warning, TEXT("ENEMY: %s == %s"), *playername, *enemyPlayer);
	if (playername == enemyPlayer)
	{
		UE_LOG(LogTemp, Warning, TEXT("ISENEMY"));
		pcontroller->IsEnemy = true;
	}

	playercount++;
		
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


UClass* AGameModeMultiplayer::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	UE_LOG(LogTemp, Warning, TEXT("GETPAWN"));
	APlayerControllerMultiplayer* pcontroller = Cast<APlayerControllerMultiplayer>(InController);
	if (pcontroller->IsEnemy)
	{
		return EnemyPawn;
	}
	else
	{
		return DefaultPawnClass;
	}
}