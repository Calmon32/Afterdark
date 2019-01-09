// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PlayerControllerMultiplayer.h"
#include "KeyPickup.h"
#include "GameFramework/Character.h"
#include "GameModeMultiplayer.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJ_API AGameModeMultiplayer : public AGameMode
{
	GENERATED_BODY()

public:

	AGameModeMultiplayer();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;


	int expectedPlayerCount;
	int playercount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Server)
	int thisIsServer;

	TArray<class APlayerControllerMultiplayer*> PlayerControllerList;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<ACharacter> EnemyPawn;

	void CloseGame();

	UPROPERTY(BlueprintReadOnly, Category = "GamePlay")
	int totalPlayers;

	UPROPERTY(BlueprintReadOnly, Category = "GamePlay")
	int PlayersOut;

	UPROPERTY(BlueprintReadOnly, Category = "GamePlay")
	int PlayersDead;

private:

	bool loaded;

	FString enemyPlayer;

	virtual bool ReadyToStartMatch_Implementation() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual FString InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal = TEXT("")) override;

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;

	TArray<AActor*> SpawnPoints;

	TArray<AActor*> KeyList;

	void RemoveExtraKeys();

	UPROPERTY(EditDefaultsOnly, Category = "Enemy")
	bool DebugMode;

	bool IsOver;
	
};
