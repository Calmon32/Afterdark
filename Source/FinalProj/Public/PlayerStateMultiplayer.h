// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PlayerStateMultiplayer.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJ_API APlayerStateMultiplayer : public APlayerState
{
	GENERATED_BODY()
	
public:

	APlayerStateMultiplayer();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = Gameplay)
	bool IsEnemy;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Gameplay)
	bool HasEscaped;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Replicated, Category = Gameplay)
	bool HasDied;

};
