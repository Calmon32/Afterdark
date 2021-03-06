
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "GameStateMultiplayer.generated.h"

UCLASS()
class FINALPROJ_API AGameStateMultiplayer : public AGameState
{
	GENERATED_BODY()

public:

	AGameStateMultiplayer();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;


	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Key")
	void KeyCaught();

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerKeyCaught();
	void ServerKeyCaught_Implementation();
	bool ServerKeyCaught_Validate();

	UFUNCTION(BlueprintCallable, Category = "Key")
	int GetKeysCaught();

	UFUNCTION(BlueprintCallable, Category = "Match")
	int GetPlayersLeft();

	int ExpectedPlayerCount;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	bool IsMatchOver;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	int NotSurvivors;

private:

	UPROPERTY(Replicated)
	int KeysCaught;

	int PlayersLeft;

};
