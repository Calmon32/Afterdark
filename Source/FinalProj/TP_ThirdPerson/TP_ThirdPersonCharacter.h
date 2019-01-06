// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PickUp.h"
#include "Door.h"
#include "TP_ThirdPersonCharacter.generated.h"

UCLASS(config=Game)
class ATP_ThirdPersonCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Battery, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* CollectionSphere;


public:
	ATP_ThirdPersonCharacter();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Camera)
	ADoor* DoorInteract;



	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	bool IsInteractButtonPressed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated, Category = Door)
	float HoldInteractTime;

	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void SetEscaped();

protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface
	UFUNCTION(BlueprintCallable, Category = "Pickups")
	void CollectPickups();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerCollectPickups();
	void ServerCollectPickups_Implementation();
	bool ServerCollectPickups_Validate();

	UFUNCTION(BlueprintImplementableEvent, Category = "Pickup")
	void CollectedPickup(APickUp* pickup);
	void CollectedPickup_Implementation(APickUp* pickup);

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerToggleCrouch();
	void ServerToggleCrouch_Implementation();
	bool ServerToggleCrouch_Validate();

	void DoThisShit_Implementation(APickUp* pickup);

	void ReleasedButton();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerReleasedButton();
	void ServerReleasedButton_Implementation();
	bool ServerReleasedButton_Validate();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Health")
	int CurrentHealth;

	UPROPERTY(Replicated, EditDefaultsOnly, BlueprintReadOnly, Category = "Health")
	int TotalHealth;


	UFUNCTION(Reliable, NetMulticast, WithValidation)
	void ClientDied();
	void ClientDied_Implementation();
	bool ClientDied_Validate();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	//Movement
	UFUNCTION(BlueprintCallable, Category=movement)
	void ToggleCrouch();

	UFUNCTION(BlueprintCallable, Category = movement)
	void DealDamage();

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float WalkSpeed;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float CrouchSpeed;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animation)
	//bool CrouchButtonDown;
private:

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", Meta = (AllowPrivateAccess = "true"))
	bool isCrouched;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Gameplay", Meta = (AllowPrivateAccess = "true"))
	float CollectionSphereRadius;

};

