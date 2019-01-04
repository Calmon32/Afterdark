// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
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

protected:

	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

public:	

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Attack")
	void BasicAttack();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerBasicAttack();
	void ServerBasicAttack_Implementation();
	bool ServerBasicAttack_Validate();


private:

	bool isCrouched;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", Meta = (AllowPrivateAccess = "true"))
	float CollectionSphereRadius;

};
