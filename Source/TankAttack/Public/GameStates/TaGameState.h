// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "TaGameState.generated.h"

class ATaPlayerState;

UCLASS()
class TANKATTACK_API ATaGameState : public AGameState
{
	GENERATED_BODY()

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void UpdateTopScore(ATaPlayerState* ScoringPlayer);

	UPROPERTY(Replicated)
	TArray<TObjectPtr<ATaPlayerState>> TopScoringPlayers;

private:
	float TopScore = 0.f;
};
