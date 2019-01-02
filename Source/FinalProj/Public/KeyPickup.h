// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Core.h"
#include "PickUp.h"
#include "KeyPickup.generated.h"

/**
 * 
 */
UCLASS()
class FINALPROJ_API AKeyPickup : public APickUp
{
	GENERATED_BODY()

public:
	AKeyPickup();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	void PickedUpBy(APawn* Pawn) override;

	
};
