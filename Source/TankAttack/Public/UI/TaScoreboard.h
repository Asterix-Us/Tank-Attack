// Copyright Vasile Bogdan, 2023. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TaScoreboard.generated.h"

class UTaScoreboardPlayerEntry;
class UVerticalBox;
class UTextBlock;
struct FTaScoreboardInfo;

UCLASS()
class TANKATTACK_API UTaScoreboard : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TitleText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> PlayerContainer;

	UPROPERTY()
	TArray<TObjectPtr<UTaScoreboardPlayerEntry>> AllPlayerWidgets;

	UPROPERTY(EditAnywhere, Category = "Ta")
	TSubclassOf<UUserWidget> ScoreboardPlayerEntryClass;

	void UpdatePlayerList(const TArray<FTaScoreboardInfo>& PlayerInfos);
};
