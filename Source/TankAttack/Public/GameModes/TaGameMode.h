// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TaGameMode.generated.h"

class ATaPlayerController;
class ATaTankCharacter;

UCLASS()
class TANKATTACK_API ATaGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ATaGameMode();

	void Tick(float DeltaTime) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	void PlayerEliminated(ATaTankCharacter* ElimCharacter, AController* VictimController, AController* AttackerController);
	void HandleRespawn(ACharacter* ElimCharacter, AController* ElimController);

	UPROPERTY(EditDefaultsOnly, Category = "Ta Game Mode")
	float WarmupTime = 5.f;
	
	float LevelStartingTime = 0.f;
	float CountdownTime = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float RespawnDelay = 3.f;

protected:
	virtual void BeginPlay() override;
	virtual void OnMatchStateSet() override;

private:
	FTimerHandle RespawnTimer;
	FTimerDelegate RespawnDelegate;
	
	void RequestRespawn(ACharacter* ElimCharacter, AController* ElimController);

	void UpdateListOfPlayers();

	UPROPERTY()
	TArray<TObjectPtr<APlayerController>> AllControllers;

public:
	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
};
