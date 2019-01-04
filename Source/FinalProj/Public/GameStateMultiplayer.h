// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameStateMultiplayer.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJ_API AGameStateMultiplayer : public AGameState
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Key")
	void KeyCaught();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerKeyCaught();
	void ServerKeyCaught_Implementation();
	bool ServerKeyCaught_Validate();

	UFUNCTION(BlueprintCallable, Category = "Key")
	int GetKeysCaught();

private:

	UPROPERTY(Replicated)
	int KeysCaught;
	
};
