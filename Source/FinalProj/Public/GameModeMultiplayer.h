// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PlayerControllerMultiplayer.h"
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

	int expectedPlayerCount;
	int playercount;

	TArray<class APlayerControllerMultiplayer*> PlayerControllerList;

private:

	bool loaded;

	virtual bool ReadyToStartMatch_Implementation() override;

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
};
