// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "UI/TaScoreboard.h"
#include "Components/VerticalBox.h"
#include "UI/TaScoreboardPlayerEntry.h"
#include "UI/Utils/TaScoreboardUtils.h"

void UTaScoreboard::UpdatePlayerList(const TArray<FTaScoreboardInfo>& PlayerInfos)
{
	// Remove any names
	PlayerContainer->ClearChildren();
	for (int32 i = 0; i < PlayerInfos.Num(); i++)
	{
		if (i > AllPlayerWidgets.Num() - 1)
		{
			APlayerController* PlayerController = GetOwningPlayer();
			if (PlayerController && ScoreboardPlayerEntryClass)
			{
				// Create the new entry
				UTaScoreboardPlayerEntry* NewEntry = CreateWidget<UTaScoreboardPlayerEntry>(PlayerController, ScoreboardPlayerEntryClass);
				AllPlayerWidgets.Add(NewEntry);

				// Update it with the right info
				NewEntry->UpdateText(PlayerInfos[i]);

				// Place the entry where it belongs on the scoreboard
				PlayerContainer->AddChild(NewEntry);
			}
		}
		else
		{
			// Update the text of the existing widget
			AllPlayerWidgets[i]->UpdateText(PlayerInfos[i]);

			// Place the entry where it belongs on the scoreboard
			PlayerContainer->AddChild(AllPlayerWidgets[i]);
		}
	}
}
