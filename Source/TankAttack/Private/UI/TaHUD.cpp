// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "UI/TaHUD.h"
#include "UI/TaCharacterOverlay.h"
#include "UI/TaElimAnnouncement.h"
#include "UI/TaStartAnnouncement.h"
#include "UI/TaPlayerName.h"
#include "UI/TaScoreboard.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/HorizontalBox.h"

void ATaHUD::AddCharacterOverlay()
{
	// Create character overlay widget
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && CharacterOverlayClass)
	{
		CharacterOverlayUI = CreateWidget<UTaCharacterOverlay>(PlayerController, CharacterOverlayClass);
		CharacterOverlayUI->AddToViewport();
	}
}

void ATaHUD::AddElimAnnouncement(FString Attacker, FString Victim)
{
	// Create elimination announcement widget
	OwningPlayer = OwningPlayer == nullptr ? GetOwningPlayerController() : OwningPlayer;
	if (OwningPlayer && ElimAnnouncementClass)
	{
		UTaElimAnnouncement* ElimAnnouncementWidget = CreateWidget<UTaElimAnnouncement>(OwningPlayer, ElimAnnouncementClass);
		if (ElimAnnouncementWidget)
		{
			ElimAnnouncementWidget->SetElimAnnouncementText(Attacker, Victim);
			ElimAnnouncementWidget->AddToViewport();

			// Set the announcements in a vertical array
			for (UTaElimAnnouncement* Msg : ElimMessages)
			{
				if (Msg && Msg->AnnouncementBox)
				{
					UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Msg->AnnouncementBox);
					if (CanvasSlot)
					{
						FVector2D Position = CanvasSlot->GetPosition();
						FVector2D NewPosition(
							CanvasSlot->GetPosition().X,
							Position.Y - CanvasSlot->GetSize().Y
						);
						CanvasSlot->SetPosition(NewPosition);
					}
				}
			}

			// Save the new announcement
			ElimMessages.Add(ElimAnnouncementWidget);

			// Start a timer to know when the hide (unparent) the current announcement
			FTimerHandle ElimMsgTimer;
			FTimerDelegate ElimMsgDelegate;
			ElimMsgDelegate.BindUFunction(this, FName("ElimAnnouncementTimerFinished"), ElimAnnouncementWidget);
			GetWorldTimerManager().SetTimer(
				ElimMsgTimer,
				ElimMsgDelegate,
				ElimAnnouncementTime,
				false
			);
		}
	}
}

void ATaHUD::AddAnnouncement()
{
	// Create announcement widget
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && AnnouncementClass)
	{
		AnnouncementUI = CreateWidget<UTaStartAnnouncement>(PlayerController, AnnouncementClass);
		AnnouncementUI->AddToViewport();
	}
}

void ATaHUD::AddNamePrompt()
{
	// UNUSED
	// Create name prompt widget
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && NamePromptClass)
	{
		NamePromptUI = CreateWidget<UTaPlayerName>(PlayerController, NamePromptClass);
		NamePromptUI->AddToViewport();
	}
}

void ATaHUD::AddScoreboard()
{
	// Create scoreboard widget
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController && ScoreboardClass)
	{
		ScoreboardUI = CreateWidget<UTaScoreboard>(PlayerController, ScoreboardClass);
		ScoreboardUI->AddToViewport();
	}
}

void ATaHUD::HandleScoreboard(bool bShow)
{
	// Show the scoreboard
	if (bShow)
	{
		ScoreboardUI->SetVisibility(ESlateVisibility::Visible);
	}
	// Hide the scoreboard
	else
	{
		ScoreboardUI->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ATaHUD::ElimAnnouncementTimerFinished(UTaElimAnnouncement* MsgToRemove)
{
	if (MsgToRemove)
	{
		MsgToRemove->RemoveFromParent();
	}
}
