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

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Key")
	void NotifyKeyCaught();

private:

	int KeysCaught;
	
};
