// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModeMultiplayer.h"
#include "PlayerControllerMultiplayer.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerStateMultiplayer.h"
#include "PlayerControllerMultiplayer.h"
#include "GameStateMultiplayer.h"
#include "Runtime/Engine/Classes/Engine/World.h"
#include "TimerManager.h"
#include "KeyPickup.h"
#include "EngineUtils.h"


AGameModeMultiplayer::AGameModeMultiplayer()
{
	PrimaryActorTick.bCanEverTick = true;

	loaded = false;

	IsOver = false;

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), SpawnPoints);
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AKeyPickup::StaticClass(), KeyList);
}

void AGameModeMultiplayer::BeginPlay() {
	Super::BeginPlay();
	// 'FCString::Atoi' converts 'FString' to 'int32' and we use the static 'ParseOption' function of the
	// 'UGameplayStatics' Class to get the correct Key from the 'OptionsString'
	UE_LOG(LogTemp, Warning, TEXT("OPTIONS: %s"), *OptionsString);
	expectedPlayerCount = FCString::Atoi(*(UGameplayStatics::ParseOption(OptionsString, "PlayerCount")));
	UE_LOG(LogTemp, Warning, TEXT("PLAYERS: %d"), expectedPlayerCount);
	thisIsServer = FCString::Atoi(*(UGameplayStatics::ParseOption(OptionsString, "IsServer")));
	UE_LOG(LogTemp, Warning, TEXT("IsServer: %d"), thisIsServer);
	enemyPlayer = UGameplayStatics::ParseOption(OptionsString, "EnemyPlayer");
	UE_LOG(LogTemp, Warning, TEXT("EnemyPlayer: %s"), *enemyPlayer);

	UE_LOG(LogTemp, Warning, TEXT("MAPNAME: %s"), *GetWorld()->GetMapName());

	AGameStateMultiplayer* gamesta = Cast<AGameStateMultiplayer>(GameState);
	gamesta->ExpectedPlayerCount = expectedPlayerCount;

	if (GetWorld()->GetMapName().Contains(FString("MAP")))
	{
		RemoveExtraKeys();
		loaded = true;
	}
}

void AGameModeMultiplayer::Tick(float DeltaTime)
{
	if (GetMatchState() == MatchState::InProgress && loaded && !IsOver)
	{
		totalPlayers = 0;
		PlayersOut = 0;
		PlayersDead = 0;
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerControllerMultiplayer* PlayerController = Cast<APlayerControllerMultiplayer>(*Iterator);
			if (PlayerController)
			{
				totalPlayers++;
				APlayerState* ps = PlayerController->PlayerState;
				APlayerStateMultiplayer* psm = Cast<APlayerStateMultiplayer>(ps);
				if (psm->IsEnemy)
				{
					totalPlayers--;
				}
				else if (psm->HasEscaped)
				{
					PlayersOut++;
				}
				else if (psm->HasDied)
				{
					PlayersDead++;
				}
			}
		}
		UE_LOG(LogTemp, Warning, TEXT("Players: %d == %d"), totalPlayers, PlayersOut + PlayersDead);
		if (PlayersOut + PlayersDead == totalPlayers)
		{
			IsOver = true;
			AGameStateMultiplayer* gs = Cast<AGameStateMultiplayer>(GameState);
			gs->IsMatchOver = true;
			gs->NotSurvivors = PlayersDead;
			UE_LOG(LogTemp, Warning, TEXT("MATCH IS OVER!!!!!!!!"));
			FTimerHandle UnusedHandle;
			GetWorldTimerManager().SetTimer(UnusedHandle, this, &AGameModeMultiplayer::CloseGame, 10.0f, false);
		}
	}
}

void AGameModeMultiplayer::CloseGame()
{
	if (Role == ROLE_Authority)
	{
		for (FConstPlayerControllerIterator Iterator = GetWorld()->GetPlayerControllerIterator(); Iterator; ++Iterator)
		{
			APlayerControllerMultiplayer* PlayerController = Cast<APlayerControllerMultiplayer>(*Iterator);
			if (PlayerController)
			{
				PlayerController->QuitGame();
			}
		}
		FGenericPlatformMisc::RequestExit(true);
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

void AGameModeMultiplayer::RemoveExtraKeys()
{
	int num = KeyList.Num() - 4;
	for (int i = 0; i < num; i++)
	{
		int index = FMath::RoundToInt(FMath::RandRange(0, KeyList.Num() - 1));
		UE_LOG(LogTemp, Warning, TEXT("REMOVEDAT: %d"), index);
		KeyList[index]->Destroy();
		KeyList.RemoveAt(index);
	}
}
