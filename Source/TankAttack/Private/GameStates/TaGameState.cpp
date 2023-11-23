// Copyright Vasile Bogdan, 2023. All Rights Reserved.


#include "GameStates/TaGameState.h"
#include "Net/UnrealNetwork.h"
#include "Characters/PlayerStates/TaPlayerState.h"

void ATaGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ATaGameState, TopScoringPlayers);
}

void ATaGameState::UpdateTopScore(ATaPlayerState* ScoringPlayer)
{
	// Update the top score
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}