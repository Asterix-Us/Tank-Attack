// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "UI/TaScoreboardPlayerEntry.h"
#include "Components/TextBlock.h"
#include "UI/Utils/TaScoreboardUtils.h"

void UTaScoreboardPlayerEntry::UpdateText(FTaScoreboardInfo NewInfo)
{
	// Update the widget info with the new info
	PlayerNameText->SetText(FText::AsCultureInvariant(NewInfo.PlayerName));
	PlayerScoreText->SetText(FText::AsCultureInvariant(FString::FromInt(static_cast<int32>(NewInfo.PlayerScore))));
}
