// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "TaHUD.generated.h"

class UUserWidget;
class UTaCharacterOverlay;
class UTaElimAnnouncement;
class UTaStartAnnouncement;
class UTaPlayerName;
class UTaScoreboard;
class APlayerController;

UCLASS()
class TANKATTACK_API ATaHUD : public AHUD
{
	GENERATED_BODY()

public:
	void AddCharacterOverlay();
	void AddElimAnnouncement(FString Attacker, FString Victim);
	void AddAnnouncement();
	void AddNamePrompt();
	void AddScoreboard();
	void HandleScoreboard(bool bShow);

private:
	UPROPERTY(EditAnywhere, Category = "Ta")
	TSubclassOf<UUserWidget> CharacterOverlayClass;
	
	UPROPERTY()
	TObjectPtr<APlayerController> OwningPlayer;
	
	UPROPERTY(EditAnywhere, Category = "Ta")
	TSubclassOf<UTaElimAnnouncement> ElimAnnouncementClass;

	UPROPERTY(EditAnywhere, Category = "Ta")
	float ElimAnnouncementTime = 6.f;

	UPROPERTY(EditAnywhere, Category = "Ta")
	TSubclassOf<UUserWidget> AnnouncementClass;

	UPROPERTY(EditAnywhere, Category = "Ta")
	TSubclassOf<UUserWidget> NamePromptClass;

	UPROPERTY(EditAnywhere, Category = "Ta")
	TSubclassOf<UUserWidget> ScoreboardClass;
	
	UFUNCTION()
	void ElimAnnouncementTimerFinished(UTaElimAnnouncement* MsgToRemove);

public:
	UPROPERTY()
	TObjectPtr<UTaCharacterOverlay> CharacterOverlayUI;
	
	UPROPERTY()
	TObjectPtr<UTaStartAnnouncement> AnnouncementUI;

	UPROPERTY()
	TObjectPtr<UTaPlayerName> NamePromptUI;

	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UTaScoreboard> ScoreboardUI;
	
	UPROPERTY()
	TArray<TObjectPtr<UTaElimAnnouncement>> ElimMessages;
};
