// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TaScoreboardPlayerEntry.generated.h"

class UTextBlock;
struct FTaScoreboardInfo;

UCLASS()
class TANKATTACK_API UTaScoreboardPlayerEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerScoreText;
	
	void UpdateText(FTaScoreboardInfo NewInfo);
};
