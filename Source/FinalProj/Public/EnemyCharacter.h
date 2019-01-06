// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Trap.h"
#include "EnemyCharacter.generated.h"

UCLASS()
class FINALPROJ_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Battery, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollectionSphere;

public:

	AEnemyCharacter();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	bool IsInteractButtonPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = Door)
	float HoldInteractTime;

protected:

	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void ReleasedButton();

	void PressedButton();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerReleasedButton();
	void ServerReleasedButton_Implementation();
	bool ServerReleasedButton_Validate();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerPressedButton();
	void ServerPressedButton_Implementation();
	bool ServerPressedButton_Validate();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerAttackSlow();
	void ServerAttackSlow_Implementation();
	bool ServerAttackSlow_Validate();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = Trap)
	int AvailableTraps;

	void ResetSpeed();

public:	

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void BasicAttack();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerBasicAttack();
	void ServerBasicAttack_Implementation();
	bool ServerBasicAttack_Validate();

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void PlaceTrap();

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<class ATrap> WhatToSpawn;

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void AttackSlow();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy", Meta = (MakeEditWidget = true))
	FVector TargetLocation;

private:

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", Meta = (AllowPrivateAccess = "true"))
	float CollectionSphereRadius;

};
