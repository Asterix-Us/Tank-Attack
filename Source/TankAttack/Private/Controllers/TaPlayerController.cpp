// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "Controllers/TaPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "UI/TaHUD.h"
#include "UI/TaStartAnnouncement.h"
#include "UI/TaCharacterOverlay.h"
#include "UI/Utils/TaScoreboardUtils.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "Gameplay/Shooting/TaCombatInterface.h"
#include "Navigation/PathFollowingComponent.h"
#include "Characters/PlayerStates/TaPlayerState.h"
#include "Components/SlateWrapperTypes.h"
#include "GameFramework/GameMode.h"
#include "GameModes/TaGameMode.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "UI/TaScoreboard.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TaPlayerController)

ATaPlayerController::ATaPlayerController()
{
	// Default values
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	CachedDestination = FVector::ZeroVector;
}

void ATaPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ATaPlayerController, CachedDestination);
	DOREPLIFETIME(ATaPlayerController, MatchState);
}

void ATaPlayerController::OnRep_Pawn()
{
	Super::OnRep_Pawn();

	// PathFollowingComp keeps a cache of NavMovementComponent, and it needs to be updated
	// after another pawn has been possessed
	if (GetLocalRole() < ROLE_Authority)
	{
		UPathFollowingComponent* PathFollowingComp = FindComponentByClass<UPathFollowingComponent>();
		if (PathFollowingComp)
		{
			PathFollowingComp->UpdateCachedComponents();
		}
	}
}

void ATaPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Save HUD reference
	HUDRef = Cast<ATaHUD>(GetHUD());

	// Save several match variables, from the server
	ServerCheckMatchState();

	// Add Input Mapping Context
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}
}

void ATaPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Handle announcement widget time
	SetHUDTime();
}

void ATaPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
	{
		// Setup mouse input events
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Completed, this, &ThisClass::OnSetDestinationReleased);
		EnhancedInputComponent->BindAction(SetDestinationClickAction, ETriggerEvent::Canceled, this, &ThisClass::OnSetDestinationReleased);

		// Setup shoot input event
		EnhancedInputComponent->BindAction(ShootAction, ETriggerEvent::Triggered, this, &ThisClass::Fire);

		// Setup mouse input events
		EnhancedInputComponent->BindAction(ScoreboardAction, ETriggerEvent::Triggered, this, &ThisClass::OnShowScoreboard);
	}
}

void ATaPlayerController::OnSetDestinationReleased()
{
	// We look for the location in the world where the player has pressed the input
	FHitResult Hit;

	// If we hit a surface, cache the location
	if (GetHitResultUnderCursor(ECC_Visibility, true, Hit))
	{
		CachedDestination = Hit.Location;
	}

	// Stop any performing movements
	StopMovement();

	// Call MoveToLocation on server
	Server_MoveToLocation(CachedDestination);

	if (!HasAuthority())
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, CachedDestination);
		
		// Spawn cursor particles locally
		if (IsLocalController())
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				this,
				FXCursor,
				CachedDestination,
				FRotator::ZeroRotator,
				FVector(1.f),
				true,
				true,
				ENCPoolMethod::None,
				true);
		}
	}
}

void ATaPlayerController::OnShowScoreboard()
{
	if (MatchState != MatchState::InProgress)
	{
		return;
	}

	// Show/hide the scoreboard
	bShowScoreboard = !bShowScoreboard;
	HUDRef->HandleScoreboard(bShowScoreboard);
}

void ATaPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void ATaPlayerController::ServerCheckMatchState_Implementation()
{
	ATaGameMode* GameMode = Cast<ATaGameMode>(UGameplayStatics::GetGameMode(this));            
	if (GameMode)                                                                                        
	{                                                                                                    
		WarmupTime = GameMode->WarmupTime;                                                               
		LevelStartingTime = GameMode->LevelStartingTime;                                                 
		MatchState = GameMode->GetMatchState();                                                          
		ClientJoin(MatchState, WarmupTime, LevelStartingTime);           
	}                                                                                                    
}

void ATaPlayerController::ClientJoin_Implementation(FName StateOfMatch, float Warmup, float StartingTime)
{
	WarmupTime = Warmup;                                            
	LevelStartingTime = StartingTime;                               
	MatchState = StateOfMatch;

	// If the match is not yet started, add some widgets
	if (HUDRef && MatchState == MatchState::WaitingToStart)     
	{
		// Add announcement widget
		HUDRef->AddAnnouncement();

		// Add scoreboard widget
		HUDRef->AddScoreboard();
		HUDRef->ScoreboardUI->SetVisibility(ESlateVisibility::Hidden);
	}                                                               
}

void ATaPlayerController::Fire()
{
	// Call interface FireWeapon from the controlled Pawn
	
	// 1st interface version
	// if (GetPawn()->GetClass()->ImplementsInterface(UTaCombatInterface::StaticClass()))
	// {
	// 	Cast<ITaCombatInterface>(GetPawn())->FireWeapon(); // C++ only
	//  ITaCombatInterface::Execute_FireWeapon(GetPawn()); // Blueprint version
	// }

	// 2nd interface version
	ITaCombatInterface* CombatInterface = Cast<ITaCombatInterface>(GetPawn());
	if (CombatInterface)
	{
		CombatInterface->FireWeapon(); // C++ only
		// CombatInterface->Execute_FireWeapon(GetPawn()); // Blueprint version
	}
}

void ATaPlayerController::OnRep_MatchState()
{
	// Check if the MatchState has changed
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
}

void ATaPlayerController::UpdatePlayerList_Implementation(const TArray<FTaScoreboardInfo>& PlayerNames)
{
	if (HUDRef && HUDRef->ScoreboardUI)
	{
		// Update the scoreboard
		HUDRef->ScoreboardUI->UpdatePlayerList(PlayerNames);
	}
}

void ATaPlayerController::SetHUDTime()
{
	// Check to see how much time has passed
	float TimeLeft = 0.f;
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;
	}
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);

	if (HasAuthority())                                                                                                              
	{                                                                                                                                
		if (GameModeRef == nullptr)                                                                                              
		{                                                                                                                            
			GameModeRef = Cast<ATaGameMode>(UGameplayStatics::GetGameMode(this));                                           
			LevelStartingTime = GameModeRef->LevelStartingTime;                                                                  
		}                                                                                                                            
		GameModeRef = GameModeRef == nullptr ? Cast<ATaGameMode>(UGameplayStatics::GetGameMode(this)) : GameModeRef;
		if (GameModeRef)                                                                                                         
		{                                                                                                                            
			SecondsLeft = FMath::CeilToInt(GameModeRef->GetCountdownTime() + LevelStartingTime);                                 
		}                                                                                                                            
	}

	// Decrease the time on the Announcement widget
	if (CountdownInt != SecondsLeft)                              
	{
		if (MatchState == MatchState::WaitingToStart)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
	}
	
	CountdownInt = SecondsLeft;
}

void ATaPlayerController::SetHUDScore(float Score)
{
	// Check to see if the HUD is valid to set the score
	HUDRef = HUDRef == nullptr ? Cast<ATaHUD>(GetHUD()) : HUDRef;        
	bool bHUDValid = HUDRef &&                                                        
		HUDRef->CharacterOverlayUI &&                                                   
		HUDRef->CharacterOverlayUI->ScoreAmount;                                        
                                                                                      
	if (bHUDValid)                                                                        
	{
		// Set the new score text on the widget
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::FloorToInt(Score));        
		HUDRef->CharacterOverlayUI->ScoreAmount->SetText(FText::FromString(ScoreText)); 
	}                                                                                     
}

void ATaPlayerController::SetHUDDefeats(int32 Defeats)
{
	// Check to see if the HUD is valid to set the defeats
	HUDRef = HUDRef == nullptr ? Cast<ATaHUD>(GetHUD()) : HUDRef;                     
	bool bHUDValid = HUDRef &&                                                           
		HUDRef->CharacterOverlayUI &&                                               
		HUDRef->CharacterOverlayUI->DefeatsAmount;
	
	if (bHUDValid)                                   
	{
		// Set the new defeats text on the widget
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);                            
		HUDRef->CharacterOverlayUI->DefeatsAmount->SetText(FText::FromString(DefeatsText));     
	}                                                                                                  
}

void ATaPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	// Check to see if the HUD is valid to set the announcement time
	HUDRef = HUDRef == nullptr ? Cast<ATaHUD>(GetHUD()) : HUDRef;                    
	bool bHUDValid = HUDRef &&                                                                    
		HUDRef->AnnouncementUI &&                                                                   
		HUDRef->AnnouncementUI->WarmupTime;
	
	if (bHUDValid)                                                                                    
	{
		// Set the time text on the widget
		if (CountdownTime < 0.f)                                                                      
		{                                                                                             
			HUDRef->AnnouncementUI->WarmupTime->SetText(FText());                                   
			return;                                                                                   
		}                                                                                             
                                                                                                   
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);                                      
		int32 Seconds = CountdownTime - Minutes * 60;                                                 
                                                                                                   
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);                 
		HUDRef->AnnouncementUI->WarmupTime->SetText(FText::FromString(CountdownText));              
	}                                                                                                 
}

void ATaPlayerController::BroadcastElim(APlayerState* Attacker, APlayerState* Victim)
{
	ClientElimAnnouncement(Attacker, Victim);
}

void ATaPlayerController::HandleMatchHasStarted()
{
	HUDRef = HUDRef == nullptr ? Cast<ATaHUD>(GetHUD()) : HUDRef;
	if (HUDRef)
	{
		// Add CharacterOverlay widget
		if (HUDRef->CharacterOverlayUI == nullptr)
		{
			HUDRef->AddCharacterOverlay();
		}

		// Hide Announcement widget
		if (HUDRef->AnnouncementUI)
		{
			HUDRef->AnnouncementUI->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void ATaPlayerController::ClientElimAnnouncement_Implementation(APlayerState* Attacker, APlayerState* Victim)
{
	// Interpret the PlayerStates of the Attacker and Victim
	APlayerState* Self = GetPlayerState<APlayerState>();
	if (Attacker && Victim && Self)
	{
		 HUDRef = HUDRef == nullptr ? Cast<ATaHUD>(GetHUD()) : HUDRef;
		 if (HUDRef)
		 {
		 	if (Attacker == Self && Victim != Self)
		 	{
		 		HUDRef->AddElimAnnouncement("You", Victim->GetPlayerName());
		 		return;
		 	}
		 	if (Victim == Self && Attacker != Self)
		 	{
		 		HUDRef->AddElimAnnouncement(Attacker->GetPlayerName(), "You");
		 		return;
		 	}
		 	if (Attacker == Victim && Attacker == Self)
		 	{
		 		HUDRef->AddElimAnnouncement("You", "yourself");
		 		return;
		 	}
		 	if (Attacker == Victim && Attacker != Self)
		 	{
		 		HUDRef->AddElimAnnouncement(Attacker->GetPlayerName(), "themselves");
		 		return;
		 	}
		 	HUDRef->AddElimAnnouncement(Attacker->GetPlayerName(), Victim->GetPlayerName());
		}
	}
}

void ATaPlayerController::Server_MoveToLocation_Implementation(const FVector_NetQuantize& DesiredLocation)
{
	// Use the native pathfinding system to reach the desired destination
	UAIBlueprintHelperLibrary::SimpleMoveToLocation(this, DesiredLocation);
}

bool ATaPlayerController::Server_MoveToLocation_Validate(const FVector_NetQuantize& DesiredLocation)
{
	return true;
}
