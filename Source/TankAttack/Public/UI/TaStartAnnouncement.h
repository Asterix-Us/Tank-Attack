// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TaStartAnnouncement.generated.h"

class UTextBlock;

UCLASS()
class TANKATTACK_API UTaStartAnnouncement : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WarmupTime;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AnnouncementText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> InfoText;
	
};
