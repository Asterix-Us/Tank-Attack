// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TaElimAnnouncement.generated.h"

class URichTextBlock;
class UHorizontalBox;

UCLASS()
class TANKATTACK_API UTaElimAnnouncement : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetElimAnnouncementText(FString AttackerName, FString VictimName);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> AnnouncementBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> AnnouncementText;
};
