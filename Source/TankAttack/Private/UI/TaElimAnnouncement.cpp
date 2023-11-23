// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#include "UI/TaElimAnnouncement.h"
#include "Components/RichTextBlock.h"

void UTaElimAnnouncement::SetElimAnnouncementText(FString AttackerName, FString VictimName)
{
	// Get the right colors for the message
	FString FinalAttackerName, FinalVictimName;
	if (AttackerName == "You")
	{
		FinalAttackerName = FString::Printf(TEXT("<RichText.Player>%s</>"), *AttackerName);
	}
	else
	{
		FinalAttackerName = FString::Printf(TEXT("<RichText.Enemy>%s</>"), *AttackerName);
	}
	
	if (VictimName == "You")
	{
		FinalVictimName = FString::Printf(TEXT("<RichText.Player>%s</>"), *VictimName);
	}
	else if (VictimName != "yourself" && VictimName != "themselves")
	{
		FinalVictimName = FString::Printf(TEXT("<RichText.Enemy>%s</>"), *VictimName);
	}
	
	// Compose the final message
	FString ElimAnnouncementText = FString::Printf(TEXT("%s eliminated %s!"), *FinalAttackerName, *FinalVictimName);
	if (AnnouncementText)
	{
		AnnouncementText->SetText(FText::FromString(ElimAnnouncementText));
	}
}
