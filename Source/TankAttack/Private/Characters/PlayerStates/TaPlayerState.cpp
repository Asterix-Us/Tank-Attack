// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "Characters/PlayerStates/TaPlayerState.h"
#include "Characters/TaTankCharacter.h"
#include "Controllers/TaPlayerController.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TaPlayerState)

void ATaPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ATaPlayerState, Defeats);
}

void ATaPlayerState::AddToScore(float ScoreAmount)
{
	// Increase the score with the given amount
	SetScore(GetScore() + ScoreAmount);
	Character = Character == nullptr ? Cast<ATaTankCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ATaPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			// Set the score info on the HUD too
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ATaPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = Character == nullptr ? Cast<ATaTankCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ATaPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			// Set the score info on the HUD too
			Controller->SetHUDScore(GetScore());
		}
	}
}

void ATaPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	// Increase the defeats with the given amount
	Defeats += DefeatsAmount;
	Character = Character == nullptr ? Cast<ATaTankCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ATaPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			// Set the defeats info on the HUD too
			Controller->SetHUDDefeats(Defeats);
		}
	}
}

void ATaPlayerState::OnRep_Defeats()
{
	Character = Character == nullptr ? Cast<ATaTankCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<ATaPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			// Set the defeats info on the HUD too
			Controller->SetHUDDefeats(Defeats);
		}
	}
}
