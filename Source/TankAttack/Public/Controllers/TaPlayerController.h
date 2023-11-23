// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UI/Utils/TaScoreboardUtils.h"
#include "TaPlayerController.generated.h"

class UTaCharacterOverlay;
class ATaHUD;
class UNiagaraSystem;
class UInputAction;
class UInputMappingContext;
class ATaGameMode;

UCLASS(DisplayName = "Ta Player Controller")
class TANKATTACK_API ATaPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATaPlayerController();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Pawn() override;

	/** FX Class that we will spawn when clicking */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ta Player Controller|Input")
	TObjectPtr<UNiagaraSystem> FXCursor;

	/** Mapping Context */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ta Player Controller|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;
	
	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ta Player Controller|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SetDestinationClickAction;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ta Player Controller|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ShootAction;

	/** Shoot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ta Player Controller|Input", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ScoreboardAction;

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void SetupInputComponent() override;

	void OnSetDestinationReleased();
	
	void OnShowScoreboard();
	
	UFUNCTION(Server, Reliable, WithValidation)
	void Server_MoveToLocation(const FVector_NetQuantize& DesiredLocation);
	
	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();
	
	UFUNCTION(Client, Reliable)
	void ClientJoin(FName StateOfMatch, float Warmup, float StartingTime);

private:
	UPROPERTY(Replicated)
	FVector CachedDestination;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<ATaHUD> HUDRef;
	
	UPROPERTY()
	TObjectPtr<ATaGameMode> GameModeRef;

	UPROPERTY()
	TObjectPtr<UTaCharacterOverlay> CharacterOverlay;

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;

	UFUNCTION()
	void OnRep_MatchState();
	
	float LevelStartingTime = 0.f;
	float WarmupTime = 0.f;
	uint32 CountdownInt = 0;

	bool bShowScoreboard = false;

public:
	void BroadcastElim(APlayerState* Attacker, APlayerState* Victim);

	UFUNCTION(Client, Reliable)
	void ClientElimAnnouncement(APlayerState* Attacker, APlayerState* Victim);

	UFUNCTION(Client, Reliable)
	void UpdatePlayerList(const TArray<FTaScoreboardInfo>& PlayerNames);
	
	void SetHUDTime();
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDAnnouncementCountdown(float CountdownTime);
	
	void OnMatchStateSet(FName State);
	void HandleMatchHasStarted();
};
