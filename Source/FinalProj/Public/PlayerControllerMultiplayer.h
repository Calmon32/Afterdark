// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerControllerMultiplayer.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJ_API APlayerControllerMultiplayer : public APlayerController
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "ServerConnect")
	void ConnectToHostLobby(FString ipAddress);
	
};
