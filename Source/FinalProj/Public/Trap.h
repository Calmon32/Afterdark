// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerControllerMultiplayer.h"
#include "GameFramework/Actor.h"
#include "Trap.generated.h"

UCLASS()
class FINALPROJ_API ATrap : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Trap, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* SphereCollider;
	
public:	
	// Sets default values for this actor's properties
	ATrap();

	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void DestroyTrap();

	APlayerControllerMultiplayer* TrappedPlayer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
