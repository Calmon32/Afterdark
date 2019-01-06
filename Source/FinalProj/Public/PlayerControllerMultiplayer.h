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

	APlayerControllerMultiplayer();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "ServerConnect")
	void ConnectToHostLobby(FString ipAddress);

	bool IsEnemy;
	bool IsInputDisabled;

	UFUNCTION(Reliable, Client, WithValidation, BlueprintCallable, Category = "Movement")
	void EnableMove();
	void EnableMove_Implementation();
	bool EnableMove_Validate();

	UFUNCTION(Reliable, Client, WithValidation, BlueprintCallable, Category = "Movement")
	void DisableMove();
	void DisableMove_Implementation();
	bool DisableMove_Validate();

	UFUNCTION(BlueprintCallable, Category = State)
	void ChangeState_Spectator();

	UFUNCTION(BlueprintCallable, Category = State)
	void ChangeState_Player();
	
};
