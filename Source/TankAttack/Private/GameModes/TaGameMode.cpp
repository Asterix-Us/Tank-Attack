// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "GameModes/TaGameMode.h"

#include "Characters/PlayerStates/TaPlayerState.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerStart.h"
#include "GameModes/TaGameMode.h"
#include "GameStates/TaGameState.h"
#include "Kismet/GameplayStatics.h"
#include "UI/Utils/TaScoreboardUtils.h"
#include "Characters/TaTankCharacter.h"
#include "Controllers/TaPlayerController.h"

ATaGameMode::ATaGameMode()
{
	// Allow for match warmup time
	bDelayedStart = true;
}

void ATaGameMode::BeginPlay()
{
	Super::BeginPlay();

	LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ATaGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If the game is waiting to start, decrease the warmup time until it's ready to start
	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
		if (CountdownTime <= 0.f)
		{
			StartMatch();
		}
	}
}

void ATaGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Register the new player who logged on the server
	AllControllers.Add(NewPlayer);
}

void ATaGameMode::OnMatchStateSet()
{
	Super::OnMatchStateSet();

	// Send all player name info to the scoreboards
	if (MatchState == MatchState::InProgress)
	{
		UpdateListOfPlayers();
	}

	// Let all player controllers know that the match state changed
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATaPlayerController* PlayerController = Cast<ATaPlayerController>(*It);
		if (PlayerController)
		{
			PlayerController->OnMatchStateSet(MatchState);
		}
	}
}

void ATaGameMode::PlayerEliminated(ATaTankCharacter* ElimCharacter, AController* VictimController, AController* AttackerController)
{
	if (AttackerController == nullptr || AttackerController->PlayerState == nullptr) return;
	if (VictimController == nullptr || VictimController->PlayerState == nullptr) return;

	ATaPlayerState* AttackerPlayerState = AttackerController ? Cast<ATaPlayerState>(AttackerController->PlayerState) : nullptr;
	ATaPlayerState* VictimPlayerState = VictimController ? Cast<ATaPlayerState>(VictimController->PlayerState) : nullptr;

	ATaGameState* CurrentGameState = GetGameState<ATaGameState>();

	// If the attacker and the victim aren't the same player
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && CurrentGameState)
	{
		// Get a local array of the top scoring players
		TArray<ATaPlayerState*> PlayersCurrentlyInTheLead;
		for (auto LeadPlayer : CurrentGameState->TopScoringPlayers)
		{
			PlayersCurrentlyInTheLead.Add(LeadPlayer);
		}

		// Increase the score of the killer
		AttackerPlayerState->AddToScore(1.f);

		// Update the top score of the attacker
		CurrentGameState->UpdateTopScore(AttackerPlayerState);

		// Let the other players know that the score has been changed
		UpdateListOfPlayers();

		// If the attacker is one of the top scoring players, let the others know! #flex
		if (CurrentGameState->TopScoringPlayers.Contains(AttackerPlayerState))
		{
			ATaTankCharacter* Leader = Cast<ATaTankCharacter>(AttackerPlayerState->GetPawn());
			if (Leader)
			{
				Leader->MulticastGainedTheLead();
			}
		}

		// If the current top scoring players array doesn't contain the previous leader, remove it's crown
		for (int32 i = 0; i < PlayersCurrentlyInTheLead.Num(); i++)
		{
			if (!CurrentGameState->TopScoringPlayers.Contains(PlayersCurrentlyInTheLead[i]))
			{
				ATaTankCharacter* Loser = Cast<ATaTankCharacter>(PlayersCurrentlyInTheLead[i]->GetPawn());
				if (Loser)
				{
					Loser->MulticastLostTheLead();
				}
			}
		}
	}

	// Increase the defeats of the victim
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
	}

	// Eliminate the victim character
	if (ElimCharacter)
	{
		ElimCharacter->Elim();
	}

	// Let the other players know via the Announcement who was killed by who
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ATaPlayerController* BlasterPlayer = Cast<ATaPlayerController>(*It);
		if (BlasterPlayer && AttackerPlayerState && VictimPlayerState)
		{
			BlasterPlayer->BroadcastElim(AttackerPlayerState, VictimPlayerState);
		}
	}
}

void ATaGameMode::HandleRespawn(ACharacter* ElimCharacter, AController* ElimController)
{
	// Finish destroying the character
	if (ElimCharacter)
	{
		ElimCharacter->Reset();
		ElimCharacter->Destroy();
	}
	
	// Start the respawn timer
	RespawnDelegate.BindUObject(this, &ThisClass::RequestRespawn, ElimCharacter, ElimController);
	GetWorldTimerManager().SetTimer(
		RespawnTimer,
		RespawnDelegate,
		RespawnDelay,
		false
	);
}

void ATaGameMode::RequestRespawn(ACharacter* ElimCharacter, AController* ElimController)
{
	// Find a random respawn point and respawn the pawn
	if (ElimController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		const int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimController, PlayerStarts[Selection]);
	}
}

void ATaGameMode::UpdateListOfPlayers()
{
	TArray<FTaScoreboardInfo> PlayerInfos;
	
	// Gather all player names
	for (APlayerController* PC : AllControllers)
	{
		PlayerInfos.Add(FTaScoreboardInfo(PC->PlayerState->GetPlayerName(), PC->PlayerState->GetScore()));
	}
	
	// Send the list to all controllers
	for (APlayerController* PC : AllControllers)
	{
		ATaPlayerController* Player = Cast<ATaPlayerController>(PC);
		Player->UpdatePlayerList(PlayerInfos);
	}
}
